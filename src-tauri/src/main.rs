use crate::mod_api::api;
use crate::mod_db::sqlite_utils::make_sure_sqlite_file_exists;
use rdev::EventType;
use tauri::menu::{Menu, MenuItem};
use tauri::{AppHandle, Manager, WebviewUrl, WebviewWindowBuilder};
use tauri::utils::config::WindowEffectsConfig;
use tauri_plugin_global_shortcut::Error::GlobalHotkey;
use tauri_plugin_global_shortcut::{ShortcutEvent, ShortcutState};

mod mod_api;
mod mod_db;
mod mod_domain;
mod mod_system;
const SEARCH_BOX_NAME: &str = "search-box";

fn main() {
    tauri::Builder::default()
        .setup(|app| {
            make_sure_sqlite_file_exists(app.handle());
            let quit_i = MenuItem::with_id(app,
                                           "quit", "退出", true, None::<&str>)?;
            let console = MenuItem::with_id(app,
                                            "console", "打开控制台", true, None::<&str>)?;
            let menu = Menu::with_items(app, &[&console, &quit_i])?;
            tauri::tray::TrayIconBuilder::new()
                .menu_on_left_click(true)
                .menu(&menu)
                .on_menu_event(|app, event| match event.id.as_ref() {
                    "quit" => {
                        println!("quit menu item was clicked");
                        app.exit(0);
                    }
                    "console" => {
                        println!("console menu item was clicked");
                        app.get_window("main").unwrap().show().unwrap()
                    }
                    _ => {
                        println!("menu item {:?} not handled", event.id);
                    }
                }
                )
                .build(app)?;
            #[cfg(desktop)]
            {
                use tauri_plugin_global_shortcut::{Code, GlobalShortcutExt, Modifiers, Shortcut};

                let search_box_shortcut = Shortcut::new(Some(Modifiers::ALT), Code::Space);
                let esc_shortcut = Shortcut::new(None, Code::Escape);
                app.handle().plugin(
                    tauri_plugin_global_shortcut::Builder::new()
                        .build(),
                )?;
                app.global_shortcut()
                    .on_shortcut(esc_shortcut,
                                 move |app, shortcut, event| {
                                     if event.state() != ShortcutState::Pressed {
                                         return;
                                     }
                                     destroy_search_box(app);
                                 }).unwrap();

                app.global_shortcut()
                    .on_shortcut(search_box_shortcut,
                                 move |app, shortcut, event| {
                                     if event.state() != ShortcutState::Pressed {
                                         return;
                                     }

                                     on_key_press(app);
                                 }).unwrap();
            }

            Ok(())
        })
        .invoke_handler(tauri::generate_handler![
            api::copy_to_clipboard,
            api::paste_into_current_window
        ])
        .run(tauri::generate_context!())
        .expect("error while running tauri application");
}

fn on_key_press(app_handle: &AppHandle) {
    let option = app_handle.get_window(SEARCH_BOX_NAME);
    match option {
        None => {
            WebviewWindowBuilder::new(app_handle, SEARCH_BOX_NAME, WebviewUrl::App(SEARCH_BOX_NAME.into()))
                .title("Input Window")
                .resizable(false)
                .decorations(false)
                .inner_size(768.0, 96.0)
                // .transparent(true)    // 透明
                .always_on_top(true) // 大小
                .build()
                .unwrap();
        }
        Some(window) => {
            if window.is_visible().unwrap_or(false) {
                window.hide().unwrap();
            } else {
                window.show().unwrap();
            }
        }
    }
}

fn destroy_search_box(app_handle: &AppHandle) {
    let search_input_window = app_handle.get_window(SEARCH_BOX_NAME);
    match search_input_window {
        Some(window) => {
            window.hide().unwrap();
        }
        None => {}
    }
}