use crate::domain::entities::provider::Provider;
use crate::domain::shared::repository::Repository;

pub trait ProviderRepository: Repository<Provider> {}
