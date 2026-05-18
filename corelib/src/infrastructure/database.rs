pub mod client_repository;

use sqlx::sqlite::SqlitePool;

pub async fn create_pool(database_url: &str) -> Result<SqlitePool, sqlx::Error> {
    SqlitePool::connect(database_url).await
}
