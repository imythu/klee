use std::collections::HashSet;
use crate::mod_api::api;
use rdev::{listen, EventType, Key};
use std::sync::{Arc, Mutex};
use std::thread;
use inputbot::KeybdKey;
use inputbot::KeybdKey::{EscapeKey, LAltKey, LShiftKey};
use rdev::Key::Escape;
use tauri::{AppHandle, CustomMenuItem, Manager, SystemTray, SystemTrayMenu, SystemTrayMenuItem};
use crate::mod_system::system_utils;
use std::ffi::{c_int, c_ulonglong, c_ushort};
use std::collections::BTreeSet;

mod mod_api;
mod mod_system;

#[cfg(target_os = "windows")]
mod key_inter;

fn main() {
    let tray_menu = SystemTrayMenu::new()
        .add_item(CustomMenuItem::new("console".to_string(), "打开控制台"))
        .add_native_item(SystemTrayMenuItem::Separator)
        .add_item(CustomMenuItem::new("quit".to_string(), "退出"));

    let tray = SystemTray::new().with_menu(tray_menu);
    tauri::Builder::default()
        .system_tray(tray)
        .invoke_handler(tauri::generate_handler![api::copy_to_clipboard, api::paste_into_current_window])
        .setup(|app| {
            let app_handle = Arc::clone(&Arc::new(Mutex::new(app.handle())));
            if system_utils::is_mac_os() {
                listen_on_macos(app_handle, on_key_press, on_escape);
            } else if system_utils::is_windows() {
                if cfg!(windows)
                {
                    unsafe {
                        key_inter::register_filter(keycode_set!([key_inter::KeyCode::KEY_LCONTROL, key_inter::KeyCode::KEY_O]), rust_filter_callback);
                    }
                }
            }
            Ok(())
        })
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

fn on_key_press(app_handle: Arc<Mutex<AppHandle>>) {
    let handle = app_handle.lock().unwrap();
    tauri::WindowBuilder::new(
        &(*handle),
        "search_input", /* the unique window label */
        tauri::WindowUrl::App("tauri://localhost/search-box".parse().unwrap()),
    )
        .title("Input Window")
        .resizable(false)
        .decorations(false)  // 无边框
        // .transparent(true)    // 透明
        .always_on_top(true)  // 保持在最前
        .inner_size(400.0, 100.0) // 大小
        .build()
        .unwrap();
}

fn on_escape(app_handle: Arc<Mutex<AppHandle>>) {
    let handle = app_handle.lock().unwrap();
    let search_input_window = (*handle).get_window("search_input");
    match search_input_window {
        Some(window) => {
            window.close().unwrap();
        }
        None => {}
    }
}

fn listen_on_macos<F1, F2>(app_handle: Arc<Mutex<AppHandle>>, on_press: F1, on_escape: F2)
where
    F1: Fn(Arc<Mutex<AppHandle>>) + Send + 'static,
    F2: Fn(Arc<Mutex<AppHandle>>) + Send + 'static,
{
    let app_handle = Arc::clone(&app_handle);
    // 在主线程中监听事件
    thread::spawn(move || {
        let key_sequence = Arc::new(Mutex::new(Vec::new()));
        let key_sequence_clone = Arc::clone(&key_sequence);
        let app_handle = Arc::clone(&app_handle);
        listen(move |event| {
            match event.event_type {
                EventType::KeyPress(key) => {
                    println!("key: {:?}", key);
                    if key == Escape {
                        let app_handle = Arc::clone(&app_handle);
                        on_escape(app_handle);
                        return;
                    }
                    let mut seq = key_sequence_clone.lock().unwrap();
                    seq.push(key);

                    if seq.len() > 3 {
                        seq.remove(0); // 保持序列长度为 3
                    }

                    if *seq == vec![Key::ShiftLeft, Key::KeyJ, Key::KeyJ]
                        || *seq == vec![Key::ShiftRight, Key::KeyJ, Key::KeyJ]
                    {
                        let app_handle = Arc::clone(&app_handle);
                        on_press(app_handle);
                        seq.clear(); // 清空序列
                    }
                }
                _ => (),
            }
        }).unwrap();
    });
}

// Rust 的回调函数
extern "C" fn rust_filter_callback(value: c_ulonglong) -> c_ushort {
    println!("Rust: filter Callback received value: {}", value);
    unsafe {
        if key_inter::iskeyintag(value, key_inter::KeyCode::KEY_LCONTROL as i32) {
            println!("Rust: filter Callback received Control Key");
        }

        if key_inter::iskeyintag(value, key_inter::KeyCode::KEY_O as i32) {
            println!("Rust: filter Callback received O Key");
        }
    }
    return 1;
}


fn listen_on_windows<F1, F2>(keys: Vec<KeybdKey>, app_handle: Arc<Mutex<AppHandle>>, on_press: F1, on_escape: F2)
where
    F1: Fn(Arc<Mutex<AppHandle>>) + Send + 'static,
    F2: Fn(Arc<Mutex<AppHandle>>) + Send + 'static,
{
    let pressed_keys = Arc::new(Mutex::new(HashSet::new()));
    let app_handle = Arc::clone(&app_handle);
    let on_press = Arc::clone(&Arc::new(Mutex::new(on_press)));
    let on_escape = Arc::clone(&Arc::new(Mutex::new(on_escape)));
    let keys = Arc::clone(&Arc::new(Mutex::new(keys)));

    thread::spawn(move || {
        for &key in &(*keys.lock().unwrap()) {
            // 每个按键绑定按下事件
            let pressed_keys_clone = Arc::clone(&pressed_keys);
            let app_handle = Arc::clone(&app_handle);
            let on_press = Arc::clone(&on_press);
            let keys = Arc::clone(&keys);

            key.bind(move || {
                println!("key: {:?}", key);
                let mut pressed = pressed_keys_clone.lock().unwrap();
                pressed.insert(key);

                // 检查所有组合键是否都被按下
                if keys.lock().unwrap().iter().all(|k| pressed.contains(k)) {
                    let app_handle = Arc::clone(&app_handle);
                    on_press.lock().unwrap()(app_handle); // 触发回调
                    pressed.clear();
                }
            });
        }

        let app_handle = Arc::clone(&app_handle);
        let on_escape = Arc::clone(&on_escape);
        EscapeKey.bind(move || {
            let app_handle = Arc::clone(&app_handle);
            on_escape.lock().unwrap()(app_handle);
        });

        // 需要调用 inputbot 的事件处理函数
        loop {
            inputbot::handle_input_events();
            std::thread::sleep(std::time::Duration::from_millis(10));  // 防止 CPU 过度消耗
        }
    });
}

