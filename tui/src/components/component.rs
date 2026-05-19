use color_eyre::Result;
use crossterm::event::Event;
use ratatui::Frame;
use ratatui::layout::Rect;

pub trait Component {
    /// Maneja los eventos de teclado/ratón. Por defecto no hace nada.
    fn handle_event(&mut self, _event: &Event) -> Result<()> {
        Ok(())
    }

    /// Dibuja el componente en el área especificada
    fn render(&mut self, f: &mut Frame, area: Rect);
}
