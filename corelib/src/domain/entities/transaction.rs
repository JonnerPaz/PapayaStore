use crate::domain::{
    entities::base_entity::BaseEntity,
    shared::entity::{Entity, EntityStatus},
};

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum TransactionKind {
    Purchase,
    Sale,
}

#[derive(Debug, Clone)]
pub struct TransactionItem {
    pub product_id: u64,
    pub quantity: u32,
    pub unit_price: f64,
}

#[derive(Debug)]
pub struct Transaction {
    base: BaseEntity,
    transaction_type: TransactionKind,
    related_id: u64,
    total: f64,
    description: String,
    items: Vec<TransactionItem>,
}

impl Transaction {
    pub fn new(
        id: u64,
        name: String,
        transaction_type: TransactionKind,
        related_id: u64,
        description: String,
    ) -> Self {
        Self {
            base: BaseEntity::new(id, name),
            transaction_type,
            related_id,
            total: 0.0,
            description,
            items: Vec::new(),
        }
    }

    pub fn transaction_type(&self) -> TransactionKind {
        self.transaction_type
    }

    pub fn related_id(&self) -> u64 {
        self.related_id
    }

    pub fn total(&self) -> f64 {
        self.total
    }

    pub fn description(&self) -> &str {
        &self.description
    }

    pub fn items(&self) -> &[TransactionItem] {
        &self.items
    }

    pub fn add_item(&mut self, item: TransactionItem) {
        self.total += item.unit_price * (item.quantity as f64);
        self.items.push(item);
    }
}

impl Entity for Transaction {
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
