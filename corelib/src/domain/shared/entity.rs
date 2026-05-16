pub trait Entity {
    fn id(&self) -> u64;
    fn name(&self) -> &str;
    fn set_name(&mut self, name: String);
    fn is_active(&self) -> EntityStatus;
    fn set_is_active(&mut self, is_active: EntityStatus);
    fn created_at(&self) -> chrono::DateTime<chrono::Utc>;
    fn updated_at(&self) -> chrono::DateTime<chrono::Utc>;
    fn set_updated_at(&mut self, updated_at: chrono::DateTime<chrono::Utc>);
}

#[derive(Debug, Copy, Clone)]
pub enum EntityStatus {
    Active,
    Inactive,
}
