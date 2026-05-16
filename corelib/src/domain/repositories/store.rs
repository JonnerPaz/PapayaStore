use crate::domain::entities::store::Store;
use crate::domain::shared::repository::{Repository, RepositoryError};

pub trait StoreRepository: Repository<Store> {
    fn find_by_rif(
        &self,
        rif: &str,
    ) -> impl std::future::Future<Output = Result<Option<Store>, RepositoryError>> + Send;
}
