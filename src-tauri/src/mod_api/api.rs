use crate::mod_system::system_utils;
use arboard::Clipboard;
use std::process::Command;

#[tauri::command]
pub fn copy_to_clipboard(text: String) -> Result<(), String> {
    println!("copy_to_clipboard: {}", &text);
    let mut clipboard = Clipboard::new().or(Err(String::from("Failed to create clipboard")))?;
    clipboard.set_text(text).map_err(stringify)?;
    Ok(())
}
#[tauri::command]
pub fn paste_into_current_window(text: String) -> Result<(), String> {
    println!("paste_into_current_window: {}", &text);
    // 判断当前操作系统
    if system_utils::is_windows() {
        // 使用 Powershell 的 SendKeys 功能模拟键盘输入
        let script = format!(
            r#"Add-Type -AssemblyName System.Windows.Forms; [System.Windows.Forms.SendKeys]::SendWait('{}')"#,
            text
        );

        // 调用 PowerShell 运行 SendKeys 命令
        Command::new("powershell")
            .arg("-Command")
            .arg(script)
            .output()
            .map_err(stringify)?;
    } else if system_utils::is_mac_os() {
        let applescript = format!(r#"tell application "System Events" to keystroke "{}""#, &text);
        Command::new("osascript")
            .arg("-e")
            .arg(applescript)
            .output()
            .map_err(stringify)?;
        println!("success")
    } else {
        println!("当前系统不是 Windows 或 macOS");
        Err("当前系统不是 Windows 或 macOS").map_err(stringify)?;
    }
    Ok(())
}

#[tauri::command]
pub(crate) fn search(text: String) -> Result<(), String> {
    println!("search: {}", &text);
    Ok(())
}

fn stringify<E: std::fmt::Debug>(e: E) -> String {
    format!("{:?}", e)
}