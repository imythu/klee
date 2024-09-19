use crate::mod_api::api;
use crate::mod_system::system_utils;
use rdev::Key::Escape;
use rdev::{listen, EventType, Key};
use rusqlite::Connection;
use std::collections::HashSet;
use std::fs::File;
use std::sync::{Arc, Mutex};
use std::thread;
use tauri::{AppHandle, CustomMenuItem, Manager, PathResolver, SystemTray, SystemTrayMenu, SystemTrayMenuItem};
use crate::mod_db::sqlite_utils::make_sure_sqlite_file_exists;

mod mod_api;
mod mod_system;
mod mod_domain;
mod mod_db;

fn main() {
    make_sure_sqlite_file_exists();
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
                // listen_on_windows(vec![LAltKey, LAltKey], app_handle, on_key_press, on_escape);
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
        tauri::WindowUrl::App("search-box".into()),
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


