pub type RepositoryError = String;

pub trait Repository<T> {
    async fn find_by_id(&self, id: u64) -> Result<Option<T>, RepositoryError>;
    async fn find_by_name(&self, name: &str) -> Result<Option<T>, RepositoryError>;
    async fn save(&self, entity: T) -> Result<T, RepositoryError>;
    async fn delete(&self, id: u64) -> Result<bool, RepositoryError>;
    async fn update(&self, id: u64, entity: T) -> Result<T, RepositoryError>;
}
