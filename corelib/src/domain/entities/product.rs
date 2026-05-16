use crate::domain::{
    entities::base_entity::BaseEntity,
    shared::entity::{Entity, EntityStatus},
};

#[derive(Debug)]
pub struct Product {
    base: BaseEntity,

    // specific attributes
    code: String,
    description: String,
    price: f64,
    stock: u64,
    min_stock: u64,
    total_sold: u64,

    // foreign key
    provider_id: u64,
}

impl Product {
    pub fn new(name: String) -> Product {
        Product {
            base: BaseEntity::new(0, name),
            code: String::new(),
            description: String::new(),
            price: 0.0,
            stock: 0,
            min_stock: 0,
            total_sold: 0,
            provider_id: 0,
        }
    }

    pub fn code(&self) -> &str {
        &self.code
    }

    pub fn set_code(&mut self, code: String) {
        self.code = code;
    }

    pub fn description(&self) -> &str {
        &self.description
    }

    pub fn set_description(&mut self, description: String) {
        self.description = description;
    }

    pub fn price(&self) -> f64 {
        self.price
    }

    pub fn set_price(&mut self, price: f64) {
        self.price = price;
    }

    pub fn stock(&self) -> u64 {
        self.stock
    }

    pub fn set_stock(&mut self, stock: u64) {
        self.stock = stock;
    }

    pub fn min_stock(&self) -> u64 {
        self.min_stock
    }

    pub fn set_min_stock(&mut self, min_stock: u64) {
        self.min_stock = min_stock;
    }

    pub fn total_sold(&self) -> u64 {
        self.total_sold
    }

    pub fn provider_id(&self) -> u64 {
        self.provider_id
    }
}

impl Entity for Product {
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
