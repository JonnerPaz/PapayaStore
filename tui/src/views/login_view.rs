use ratatui::{Frame, layout::Rect};

use crate::components::component::Component;

pub struct LoginView {}

impl LoginView {
    pub fn new() -> Self {
        Self {}
    }
}

impl Component for LoginView {
    fn render(&mut self, f: &mut Frame, area: Rect) {}

    // fn handle_events(&mut self) {}
}
