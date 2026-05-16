use crate::domain::entities::transaction::{Transaction, TransactionItem, TransactionKind};
use crate::domain::repositories::{
    client::ClientRepository, product::ProductRepository, store::StoreRepository,
    transaction::TransactionRepository,
};

pub struct TransactionService<T, P, S, C>
where
    T: TransactionRepository,
    P: ProductRepository,
    S: StoreRepository,
    C: ClientRepository,
{
    transaction_repo: T,
    product_repo: P,
    store_repo: S,
    client_repo: C,
}

impl<T, P, S, C> TransactionService<T, P, S, C>
where
    T: TransactionRepository,
    P: ProductRepository,
    S: StoreRepository,
    C: ClientRepository,
{
    pub fn new(transaction_repo: T, product_repo: P, store_repo: S, client_repo: C) -> Self {
        Self {
            transaction_repo,
            product_repo,
            store_repo,
            client_repo,
        }
    }

    pub async fn process_sale(
        &self,
        client_id: u64,
        items: Vec<TransactionItem>,
    ) -> Result<Transaction, String> {
        let client = self.client_repo.find_by_id(client_id).await?;
        if client.is_none() {
            return Err("Client not found".to_string());
        }

        let mut transaction = Transaction::new(
            0,
            "Sale".to_string(),
            TransactionKind::Sale,
            client_id,
            "Sale transaction".to_string(),
        );

        for item in items {
            transaction.add_item(item);
        }

        self.transaction_repo.save(transaction).await
    }
}
