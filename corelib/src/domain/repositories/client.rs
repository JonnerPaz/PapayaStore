use crate::domain::entities::client::Client;
use crate::domain::shared::repository::Repository;

pub trait ClientRepository: Repository<Client> {}
