use color_eyre::{Result, eyre::Ok};
use crossterm::event::{self, Event, KeyCode, KeyEventKind};
use ratatui::{Terminal, backend::CrosstermBackend, layout::Alignment, widgets::Paragraph};
use std::io;

use crate::components::component::Component;
use crate::views::greeting::Greeting;

// Routing of the application.
pub enum ActiveView {
    Greeting,
    Catalog,
    Search,
    Cart,
}

pub struct App {
    pub should_quit: bool,
    pub active_view: ActiveView,
    pub greeting: Greeting,
}

impl App {
    pub fn new() -> Self {
        Self {
            should_quit: false,
            active_view: ActiveView::Greeting,
            greeting: Greeting {
                title: String::new(),
            },
        }
    }

    pub fn run(&mut self, terminal: &mut Terminal<CrosstermBackend<io::Stdout>>) -> Result<()> {
        while !self.should_quit {
            terminal.draw(|f| match self.active_view {
                ActiveView::Greeting => self.greeting.render(f, f.area()),
                ActiveView::Catalog => {
                    let text = Paragraph::new(
                        "\n\nCatálogo (En construcción)\n\nPresiona 'Esc' para volver al inicio.",
                    )
                    .alignment(Alignment::Center);
                    f.render_widget(text, f.area());
                }
                ActiveView::Search => {
                    let text = Paragraph::new(
                        "\n\nBúsqueda (En construcción)\n\nPresiona 'Esc' para volver al inicio.",
                    )
                    .alignment(Alignment::Center);
                    f.render_widget(text, f.area());
                }
                ActiveView::Cart => {
                    let text = Paragraph::new(
                        "\n\nCarrito (En construcción)\n\nPresiona 'Esc' para volver al inicio.",
                    )
                    .alignment(Alignment::Center);
                    f.render_widget(text, f.area());
                }
            })?;

            // Handle events like key presses
            // if this line is removed, the app will not react to key presses
            // so even the app itself cannot be closed
            self.handle_events()?;
        }

        Ok(())
    }

    pub fn handle_events(&mut self) -> Result<()> {
        if event::poll(std::time::Duration::from_millis(50))? {
            let ev = event::read()?;

            if let Event::Key(key) = ev {
                if key.kind == KeyEventKind::Press {
                    match key.code {
                        // Global navigation
                        KeyCode::Char('q') | KeyCode::Char('4') => self.should_quit = true,
                        KeyCode::Char('1') => self.active_view = ActiveView::Catalog,
                        KeyCode::Char('2') => self.active_view = ActiveView::Search,
                        KeyCode::Char('3') => self.active_view = ActiveView::Cart,
                        KeyCode::Esc => self.active_view = ActiveView::Greeting,
                        _ => {}
                    }
                }
            }

            // Delegate the event to the active component
            match self.active_view {
                ActiveView::Greeting => self.greeting.handle_event(&ev)?,
                _ => {}
            }
        }

        Ok(())
    }
}
