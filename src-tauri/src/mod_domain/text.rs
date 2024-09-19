use diesel::{Insertable, Queryable, Selectable};

// 文本结构体
#[derive(Queryable, Selectable, Insertable, Debug)]
#[diesel(table_name = crate::mod_domain::t_text)]
#[diesel(check_for_backend(diesel::sqlite::Sqlite))]
pub(crate) struct Text {
    // id
    id: i32,
    // text
    text: String,
    // short_text
    short_text: String,
}