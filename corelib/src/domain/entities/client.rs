use std::result::Result;

use crate::domain::{
    entities::base_entity::BaseEntity,
    shared::entity::{Entity, EntityStatus},
};

#[derive(Debug)]
pub struct Client {
    base: BaseEntity,
    email: String,
    phone: String,
    address: String,
}

impl Client {
    pub fn new(
        id: u64,
        name: String,
        email: String,
        phone: Option<String>,
        address: Option<String>,
    ) -> Result<Client, String> {
        if !Client::is_valid_email(&email) {
            return Err("Invalid email".to_string());
        }

        // Default values
        let phone = phone.unwrap_or_default();
        let address = address.unwrap_or_default();

        Ok(Client {
            base: BaseEntity::new(id, name),
            email,
            phone,
            address,
        })
    }

    pub fn email(&self) -> &str {
        &self.email
    }

    pub fn phone(&self) -> Option<&str> {
        if self.phone.is_empty() {
            None
        } else {
            Some(&self.phone)
        }
    }

    pub fn address(&self) -> Option<&str> {
        if self.address.is_empty() {
            None
        } else {
            Some(&self.address)
        }
    }

    fn is_valid_email(email: &str) -> bool {
        let email = email.trim();

        if email.contains(char::is_whitespace) {
            return false;
        }

        let parts = email.split('@').collect::<Vec<&str>>();
        if parts.len() != 2 {
            return false;
        }

        let local_part = parts[0];
        let domain_part = parts[1];
        if local_part.is_empty() || domain_part.is_empty() {
            return false;
        }

        if domain_part.starts_with('.') || domain_part.ends_with('.') || domain_part.contains("..")
        {
            return false;
        }

        true
    }
}

impl Entity for Client {
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
