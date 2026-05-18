use crate::domain::entities::client::Client;
use crate::domain::repositories::client::ClientRepository;
use crate::domain::shared::repository::{Repository, RepositoryError};
use sqlx::SqlitePool;

pub struct SqlxClientRepository {
    pool: SqlitePool,
}

impl SqlxClientRepository {
    pub fn new(pool: SqlitePool) -> Self {
        Self { pool }
    }
}

impl Repository<Client> for SqlxClientRepository {
    async fn find_by_id(&self, id: u64) -> Result<Option<Client>, RepositoryError> {
        let id_i64 = id as i64;
        let row = sqlx::query!(
            r#"SELECT id, name, email, phone, address FROM clients WHERE id = ?"#,
            id_i64
        )
        .fetch_optional(&self.pool)
        .await
        .map_err(|e| e.to_string())?;

        match row {
            Some(r) => {
                let client = Client::new(r.id as u64, r.name, r.email, r.phone, r.address)
                    .map_err(|e| e.to_string())?;
                Ok(Some(client))
            }
            None => Ok(None),
        }
    }

    async fn find_by_name(&self, name: &str) -> Result<Option<Client>, RepositoryError> {
        let row = sqlx::query!(
            r#"SELECT id, name, email, phone, address FROM clients WHERE name = ?"#,
            name
        )
        .fetch_optional(&self.pool)
        .await
        .map_err(|e| e.to_string())?;

        match row {
            Some(r) => {
                let client = Client::new(r.id as u64, r.name, r.email, r.phone, r.address)
                    .map_err(|e| e.to_string())?;
                Ok(Some(client))
            }
            None => Ok(None),
        }
    }

    async fn save(&self, entity: Client) -> Result<Client, RepositoryError> {
        use crate::domain::shared::entity::Entity;

        let is_active = matches!(
            entity.is_active(),
            crate::domain::shared::entity::EntityStatus::Active
        );

        let name = entity.name();
        let email = entity.email();
        let phone = entity.phone();
        let address = entity.address();

        let row = sqlx::query!(
            r#"
            INSERT INTO clients (name, email, phone, address, is_active) 
            VALUES (?, ?, ?, ?, ?) RETURNING id
            "#,
            name,
            email,
            phone,
            address,
            is_active
        )
        .fetch_one(&self.pool)
        .await
        .map_err(|e| e.to_string())?;

        // In a real scenario we'd reconstruct the entity with the new ID
        let new_entity = Client::new(
            row.id as u64,
            entity.name().to_string(),
            entity.email().to_string(),
            entity.phone().map(String::from),
            entity.address().map(String::from),
        )
        .unwrap();

        Ok(new_entity)
    }

    async fn delete(&self, id: u64) -> Result<bool, RepositoryError> {
        let id_i64 = id as i64;
        let result = sqlx::query!(r#"DELETE FROM clients WHERE id = ?"#, id_i64)
            .execute(&self.pool)
            .await
            .map_err(|e| e.to_string())?;

        Ok(result.rows_affected() > 0)
    }

    async fn update(&self, _id: u64, entity: Client) -> Result<Client, RepositoryError> {
        // Implementation for update
        Ok(entity)
    }
}

impl ClientRepository for SqlxClientRepository {}
