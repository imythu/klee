pub fn is_mac_os() -> bool {
    cfg!(target_os = "macos")
}
pub fn is_windows() -> bool {
    cfg!(target_os = "windows")
}
