use crate::domain::entities::base_entity::BaseEntity;
use crate::domain::entities::product::Product;
use crate::domain::shared::entity::{Entity, EntityStatus};

pub struct Provider {
    base: BaseEntity,
    rif: String,
    address: String,
    products: Vec<Product>,
}

impl Provider {
    pub fn new(
        id: u64,
        name: String,
        rif: String,
        address: Option<String>,
        products: Option<Vec<Product>>,
    ) -> Self {
        let address = address.unwrap_or_default();
        let products = products.unwrap_or_default();

        Self {
            base: BaseEntity::new(id, name),
            rif,
            address,
            products,
        }
    }
}

impl Entity for Provider {
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
