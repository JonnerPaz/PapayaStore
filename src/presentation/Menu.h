struct OpcionMenu {
    const char* descripcion;
    void (*accion)();
};

// clase abstracta
class Menu {

  public:
    virtual void mostrarMenu() = 0;
    virtual void drawMenu(const char* title, OpcionMenu options[], int numOptions,
                          const char* texToExit = "Volver al menú principal") = 0;
    virtual ~Menu();
};
