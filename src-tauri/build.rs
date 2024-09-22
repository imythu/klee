use std::env;
use std::fs;
use std::path::PathBuf;

fn main() {
    tauri_build::build();

    if cfg!(windows)
    {
        // 告诉 cargo 动态库所在的路径
        println!("cargo:rustc-link-search=native=Lib/bin/Debug/");
        
        // 告诉 cargo 要链接的库名（库名不需要扩展名）
        println!("cargo:rustc-link-lib=dylib=winkeyhook");  // libmylib.so 或 mylib.dll

        // 指定动态库的路径 (根据实际路径修改)
        let library_path = PathBuf::from("Lib/bin/Debug/winkeyhook.dll");

        // 打印出动态库的绝对路径或错误信息以及原始路径
        match library_path.canonicalize() {
            Ok(absolute_path) => {
                println!("Absolute path of library: {}", absolute_path.display());
            },
            Err(e) => {
                // 如果 `canonicalize` 失败，尝试构建一个基于当前目录的绝对路径
                let current_dir = env::current_dir().unwrap();
                let constructed_absolute_path = current_dir.join(&library_path);
                println!(
                    "Failed to get absolute path for library_path '{}': {}, constructed absolute path: '{}'",
                    library_path.display(), e, constructed_absolute_path.display()
                );
            }
        }


        // 获取构建目标目录
        let out_dir = env::var("OUT_DIR").unwrap();

        // 创建 PathBuf 实例，确保它的生命周期足够长
        let out_path = PathBuf::from(out_dir);  // 注意这里不使用引用，直接转为 PathBuf
    
        // 获取生成的可执行文件目录（例如 target/debug 或 target/release）
        let target_dir = out_path
            .parent().unwrap()
            .parent().unwrap()
            .parent().unwrap();
    
        // 将动态库复制到生成的可执行文件目录
        let target_library_path = target_dir.join("winkeyhook.dll");
        fs::copy(library_path, target_library_path).expect("Failed to copy dynamic library");
    }

}
