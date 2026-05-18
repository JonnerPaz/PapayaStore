use crate::app::App;
use ratatui::{
    Frame,
    layout::{Alignment, Constraint, Direction, Layout},
    widgets::{Block, BorderType, Borders, Paragraph},
};

pub fn render(_app: &App, f: &mut Frame) {
    let size = f.area();

    let block = Block::default()
        .borders(Borders::ALL)
        .title(" PapayaStore ")
        .title_alignment(Alignment::Left)
        .border_type(BorderType::Rounded);

    let text = Paragraph::new("\n\nBienvenido a PapayaStore\n\nPresiona 'q' para salir")
        .block(block)
        .alignment(Alignment::Center);

    let vertical_layout = Layout::default()
        .direction(Direction::Vertical)
        .constraints([
            Constraint::Percentage(25),
            Constraint::Percentage(50),
            Constraint::Percentage(25),
        ])
        .split(size);

    let horizontal_layout = Layout::default()
        .direction(Direction::Horizontal)
        .constraints([
            Constraint::Percentage(20),
            Constraint::Percentage(60),
            Constraint::Percentage(20),
        ])
        .split(vertical_layout[1]);

    f.render_widget(text, horizontal_layout[1]);
}
