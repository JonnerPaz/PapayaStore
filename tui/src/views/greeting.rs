use crate::components::component::Component;

use ratatui::{
    Frame,
    layout::{Alignment, Direction, Layout, Rect},
    prelude::*,
    widgets::{Block, Borders, Paragraph},
};

pub struct Greeting {
    pub title: String,
}

impl Component for Greeting {
    fn render(&mut self, f: &mut Frame, area: Rect) {
        let block = Block::default().borders(Borders::ALL);

        let vertical_chunks = Layout::default()
            .direction(Direction::Vertical)
            .constraints([
                Constraint::Percentage(5),
                Constraint::Length(10),
                Constraint::Min(0), // Fills the remaining space
            ])
            .split(area);

        let text = Paragraph::new(
            r"
__________                                       _________ __                        
\______   \_____  ___________  ___.__._____     /   _____//  |_  ___________   ____  
 |     ___/\__  \ \____ \__  \<   |  |\__  \    \_____  \\   __\/  _ \_  __ \_/ __ \ 
 |    |     / __ \|  |_> > __ \\___  | / __ \_  /        \|  | (  <_> )  | \/\  ___/ 
 |____|    (____  /   __(____  / ____|(____  / /_______  /|__|  \____/|__|    \___  >
                \/|__|       \/\/          \/          \/                         \/ 
",
        )
        .block(block)
        .alignment(Alignment::Center);

        f.render_widget(text, vertical_chunks[1]);

        // Draw options below the logo
        let options_text = Paragraph::new(
            "1. Ver Catálogo\n\
             2. Buscar Producto\n\
             3. Mi Carrito\n\
             4 (o 'q'). Salir",
        )
        .alignment(Alignment::Center);

        let bottom_layout = Layout::default()
            .direction(Direction::Vertical)
            .constraints([
                Constraint::Length(2), // margin-top from logo
                Constraint::Min(0),    // Space for text
            ])
            .split(vertical_chunks[2]);

        f.render_widget(options_text, bottom_layout[1]);
    }
}
