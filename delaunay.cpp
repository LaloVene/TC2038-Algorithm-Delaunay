
// triangulación de Delaunay del conjunto de puntos usando programación CGAL, diagrama de voronoi
// Si le preocupa el algoritmo de Delaunay, consulte la implementación de la triangulación de Delaunay por parte de CGAL. ​​La implementación de la triangulación de Delaunay por parte de CGAL es incremental.
// Este programa se enfoca en acceder a la estructura de datos de Delaunay en CGAL, consulte las funciones points_triangulation ()

//SudoLeo 2010/7/20
//CGAL required, GLUT required

#include <stdio.h>
#include <stdlib.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
//#include <CGAL/Triangulation_euclidean_traits_xy_3.h>
#include <CGAL/Projection_traits_xy_3.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <GLUT/glut.h>
#include <iostream>
#include <cmath>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_2<K> Delaunay;
typedef Delaunay::Vertex_handle Vertex_handle;

typedef K::Point_2 Point;

std::vector<Point> vertices;

int global_w, global_h;
int tri_state = 0;

void points_draw()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();

    std::vector<Point>::iterator iter;
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(5);
    glBegin(GL_POINTS);
    for (iter = vertices.begin(); iter != vertices.end(); iter++)
        glVertex2i(iter->hx(), iter->hy());
    glEnd();

    glPopMatrix();
    glutSwapBuffers();
}

void points_add_point(int x, int y)
{
    vertices.push_back(Point(x, global_h - y));
}

void points_clear()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glPopMatrix();
    glutSwapBuffers();

    vertices.clear();
    tri_state = 0;
}

void read_file() // Leer datos del conjunto de puntos de un archivo, utilizado durante la depuración
{
    FILE *f;
    f = freopen("landmarks.txt", "r", stdin);

    int a, b;
    while (std::cin >> a >> b)
    {
        vertices.push_back(Point(a, b));
    }
    std::cout << "In total: " << vertices.size() << " points were read." << std::endl;

    fclose(f);
}

void points_triangulation()
{
    Delaunay dt; // Estructura de datos de Delaunay, que representa una y solo una triangulación de los datos actuales, consulte CGAL_manual para obtener detalles.

    dt.insert(vertices.begin(), vertices.end()); // datos de entrada

    points_draw(); // puntos_draw () la función ha llamado glutSwapBuffers () una vez, esta función llama a glutSwapBuffers () nuevamente
                   // Llame a glutSwapBuffers () dos veces en el dibujo de un cuadro. Aunque no tiene ningún efecto en este ejemplo, hay algunos problemas. Este no es el enfoque de este artículo y puede ignorarse por el momento.

    glPushMatrix();

    Delaunay ::Finite_faces_iterator fit; // Iterar sobre todas las caras (caras finitas) de Delaunay y dibujar los bordes de cada cara
    glColor3f(0.0, 0.0, 1.0);
    for (fit = dt.finite_faces_begin(); fit != dt.finite_faces_end(); fit++)
    {
        glBegin(GL_LINE_LOOP);
        glVertex2i(fit->vertex(0)->point().hx(), fit->vertex(0)->point().hy());
        glVertex2i(fit->vertex(1)->point().hx(), fit->vertex(1)->point().hy());
        glVertex2i(fit->vertex(2)->point().hx(), fit->vertex(2)->point().hy());
        glEnd();
    } // Complete el dibujo de la triangulación de Delaunay, diagrama de Delaunay

    Delaunay ::Edge_iterator eit; // Iterar sobre todos los bordes de Delaunay, dibujar un gráfico dual del gráfico de Delaunay, es decir, el gráfico de Voronoi

    glEnable(GL_LINE_STIPPLE); // Usa el modo punteado, es decir, usa una línea de puntos para dibujar el diagrama de Voronoi
    glLineStipple(1, 0x3333);
    glColor3f(0.0, 0.0, 0.0);

    for (eit = dt.edges_begin(); eit != dt.edges_end(); eit++)
    {
        CGAL ::Object o = dt.dual(eit); // El borde correspondiente al borde eit en su gráfico dual

        if (CGAL ::object_cast<K ::Segment_2>(&o)) // Si este borde es un segmento de línea, dibuje el segmento de línea
        {
            glBegin(GL_LINES);
            glVertex2i(CGAL::object_cast<K::Segment_2>(&o)->source().hx(), CGAL::object_cast<K::Segment_2>(&o)->source().hy());
            glVertex2i(CGAL::object_cast<K::Segment_2>(&o)->target().hx(), CGAL::object_cast<K::Segment_2>(&o)->target().hy());
            glEnd();
        }
        else if (CGAL ::object_cast<K ::Ray_2>(&o)) // Si este borde es un rayo, dibuje el rayo
        {
            glBegin(GL_LINES);
            glVertex2i(CGAL::object_cast<K::Ray_2>(&o)->source().hx(), CGAL::object_cast<K::Ray_2>(&o)->source().hy());
            glVertex2i(CGAL::object_cast<K::Ray_2>(&o)->point(1).hx(), CGAL::object_cast<K::Ray_2>(&o)->point(1).hy());
            glEnd();
        }
    }
    glDisable(GL_LINE_STIPPLE); // Deshabilita el modo de punteado

    glPopMatrix();
    glutSwapBuffers();

    tri_state = 1; // triangulación completa, establecer el estado en 1
}

void display(void)
{
}

void init(void)
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
}

void reshape(int w, int h)
{
    global_w = w;
    global_h = h;
    points_clear();

    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    glOrtho(0, w, 0, h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    {
        if (tri_state == 1)
            points_clear();
        else
        {
            //points_add_point(x,y);
            read_file();
            points_draw();
        }
    }
    if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
    {
        if (tri_state == 1)
            points_clear();
        else
            points_triangulation();
    }
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key)
    {
    case 27:
        exit(0);
        break;
    }
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow(argv[0]);
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutMouseFunc(mouse);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
