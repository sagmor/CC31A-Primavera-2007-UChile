/*
 * Funciones para generar comandos a wish
 * Ver ejemplo.c como ejemplo de uso
 * se espera que la salida vaya a un pipe a wish como en:
 * ./ejemplo | wish
 */

/* Inicializa ventana de maxx X maxy pixeles */
void init_w(int maxx, int maxy);

/* Genera un mensaje en la ventana en la posicion (x, y) y espera un enter
   antes de cerrar la ventana del wish
*/
void close_w(char *bye, int x, int y);

/* Crea un objeto en x,y de ancho gordo de tipo type y de color color
   retorna el identificador del objeto
 */
int create_obj_w(int x, int y, int gordo, char *type, char *color);

/* Mueve el objeto de identificador item en delta dx y delta dy */
void move_w(int item, int dx, int dy);

/* Mueve el objeto a las coordenadas x,y y lo deja de ancho gordo */
void coords_w(int item, int x, int y, int gordo);
