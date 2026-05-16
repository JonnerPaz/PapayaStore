use crate::domain::entities::product::Product;
use crate::domain::shared::repository::Repository;

pub trait ProductRepository: Repository<Product> {}
