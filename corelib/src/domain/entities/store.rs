use crate::domain::{
    entities::base_entity::BaseEntity,
    shared::entity::{Entity, EntityStatus},
};

#[derive(Debug)]
pub struct Store {
    base: BaseEntity,
    rif: String,
    active_products_count: u32,
    active_providers_count: u32,
    active_clients_count: u32,
    active_transactions_count: u32,
    total_sales_amount: f64,
    total_purchases_amount: f64,
}

impl Entity for Store {
    fn id(&self) -> u64 {
        self.base.id()
    }

    fn name(&self) -> &str {
        self.base.name()
    }

    fn set_name(&mut self, name: String) {
        self.base.set_name(name);
    }

    fn is_active(&self) -> EntityStatus {
        self.base.is_active()
    }

    fn set_is_active(&mut self, is_active: EntityStatus) {
        self.base.set_is_active(is_active);
    }

    fn created_at(&self) -> chrono::DateTime<chrono::Utc> {
        self.base.created_at()
    }

    fn updated_at(&self) -> chrono::DateTime<chrono::Utc> {
        self.base.updated_at()
    }

    fn set_updated_at(&mut self, updated_at: chrono::DateTime<chrono::Utc>) {
        self.base.set_updated_at(updated_at);
    }
}

impl Store {
    pub fn new(id: u64, name: String, rif: String) -> Self {
        Self {
            base: BaseEntity::new(id, name),
            rif,
            active_products_count: 0,
            active_providers_count: 0,
            active_clients_count: 0,
            active_transactions_count: 0,
            total_sales_amount: 0.0,
            total_purchases_amount: 0.0,
        }
    }

    pub fn rif(&self) -> &str {
        &self.rif
    }

    pub fn active_products_count(&self) -> u32 {
        self.active_products_count
    }

    pub fn active_providers_count(&self) -> u32 {
        self.active_providers_count
    }

    pub fn active_clients_count(&self) -> u32 {
        self.active_clients_count
    }

    pub fn active_transactions_count(&self) -> u32 {
        self.active_transactions_count
    }

    pub fn total_sales_amount(&self) -> f64 {
        self.total_sales_amount
    }

    pub fn total_purchases_amount(&self) -> f64 {
        self.total_purchases_amount
    }
}
