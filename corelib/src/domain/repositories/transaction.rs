use crate::domain::entities::transaction::Transaction;
use crate::domain::shared::repository::{Repository, RepositoryError};

pub trait TransactionRepository: Repository<Transaction> {
    fn find_by_related_id(
        &self,
        related_id: u64,
    ) -> impl std::future::Future<Output = Result<Vec<Transaction>, RepositoryError>> + Send;
}
