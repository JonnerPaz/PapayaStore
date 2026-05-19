use crate::components::component::Component;
use color_eyre::Result;
use ratatui::text::Text;

pub struct Button {
    pub label: String,
    pub is_pressed: bool,
}

impl Button {
    pub fn new(label: String) -> Self {
        Self {
            label,
            is_pressed: false,
        }
    }
}

impl Component for Button {
    fn render(&mut self, f: &mut ratatui::Frame, area: ratatui::layout::Rect) {
        let block = ratatui::widgets::Block::default().borders(ratatui::widgets::Borders::ALL);

        let paragraph = ratatui::widgets::Paragraph::new(self.label.clone())
            .block(block)
            .alignment(ratatui::layout::Alignment::Center);

        f.render_widget(paragraph, area);
    }

    // fn handle_event(&mut self, _event: &crossterm::event::Event) {}
}
