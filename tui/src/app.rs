use color_eyre::Result;
use crossterm::event::{self, Event, KeyCode, KeyEventKind};

pub struct App {
    pub should_quit: bool,
}

impl App {
    pub fn new() -> Self {
        Self {
            should_quit: false,
        }
    }

    pub fn handle_events(&mut self) -> Result<()> {
        if event::poll(std::time::Duration::from_millis(50))? {
            if let Event::Key(key) = event::read()? {
                if key.kind == KeyEventKind::Press {
                    match key.code {
                        KeyCode::Char('q') => self.should_quit = true,
                        _ => {}
                    }
                }
            }
        }
        Ok(())
    }
}
