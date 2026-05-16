use crate::domain::entities::client::Client;
use crate::domain::repositories::client::ClientRepository;

pub struct ClientService<C: ClientRepository> {
    client_repo: C,
}

impl<C: ClientRepository> ClientService<C> {
    pub fn new(client_repo: C) -> Self {
        Self { client_repo }
    }

    pub async fn register_client(
        &self,
        name: String,
        email: String,
        phone: Option<String>,
        address: Option<String>,
    ) -> Result<Client, String> {
        let client = Client::new(0, name, email, phone, address)?;
        
        self.client_repo.save(client).await
    }
}
