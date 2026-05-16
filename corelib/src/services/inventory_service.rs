use crate::domain::entities::product::Product;
use crate::domain::repositories::product::ProductRepository;

pub struct InventoryService<P: ProductRepository> {
    product_repo: P,
}

impl<P> InventoryService<P>
where
    P: ProductRepository,
{
    pub fn new(product_repo: P) -> Self {
        Self { product_repo }
    }

    pub async fn register_product(&self, name: String, price: f64) -> Result<Product, String> {
        let mut product = Product::new(name);
        product.set_price(price);

        self.product_repo.save(product).await
    }
}
