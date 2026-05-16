use crate::domain::shared::entity::*;

#[derive(Debug)]
pub struct BaseEntity {
    id: u64,
    name: String,
    is_active: EntityStatus,
    created_at: chrono::DateTime<chrono::Utc>,
    updated_at: chrono::DateTime<chrono::Utc>,
}

impl Entity for BaseEntity {
    fn id(&self) -> u64 {
        self.id
    }

    fn name(&self) -> &str {
        &self.name
    }

    fn set_name(&mut self, name: String) {
        self.name = name;
    }

    fn is_active(&self) -> EntityStatus {
        self.is_active
    }

    fn set_is_active(&mut self, is_active: EntityStatus) {
        self.is_active = is_active;
    }

    fn created_at(&self) -> chrono::DateTime<chrono::Utc> {
        self.created_at
    }

    fn updated_at(&self) -> chrono::DateTime<chrono::Utc> {
        self.updated_at
    }

    fn set_updated_at(&mut self, updated_at: chrono::DateTime<chrono::Utc>) {
        self.updated_at = updated_at;
    }
}

impl BaseEntity {
    pub fn new(id: u64, name: String) -> Self {
        Self {
            id,
            name,
            is_active: EntityStatus::Active,
            created_at: chrono::Utc::now(),
            updated_at: chrono::Utc::now(),
        }
    }
}
