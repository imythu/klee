use diesel::connection::SimpleConnection;
use diesel::{Connection, SqliteConnection};
use lazy_static::lazy_static;
use std::fs::File;
use std::path::Path;

const DB_PATH: &str = "klee/data.db";

lazy_static! {
    pub(crate) static ref SQLITE_CONN: SqliteConnection = {
        let db_path = Path::new(DB_PATH);
        SqliteConnection::establish(&db_path.to_string_lossy()).unwrap()
    };
}


// 获取 tauri 运行目录，并检查 sqlite 文件是否存在，不存在则创建 sqlite 文件
pub(crate) fn make_sure_sqlite_file_exists() {
    let dir = tauri::api::path::data_dir().unwrap();
    let sqlite_path = dir.join(DB_PATH);
    if !sqlite_path.exists() {
        File::create(&sqlite_path).unwrap();
        // 建表
        (&SQLITE_CONN).batch_execute(r#"
        create table if not exists t_text(
            id integer primary key autoincrement,
            text not null,
            short_text not null,
            create_time datetime default current_timestamp,
            update_time datetime default current_timestamp
        );
        "#).unwrap()
    }
}