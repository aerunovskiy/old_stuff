/*
 * Описать класс, представляющий треугольник. Предусмотреть методы для создания объектов,
 * вычисления площади, периметра и точки пересечения медиан.
 * Описать свойства для получения состояния объекта.
 */

package lab4;

public class Figure
{
    public static class Triangle
    {
        public static class Point
        {
            Point(int x, int y)
            {
                this._x = x;
                this._y = y;
            }

            int _x;
            int _y;
        }

        public Triangle(Point A, Point B, Point C) throws Exception
        {
            _points[0] = A;
            _points[1] = B;
            _points[2] = C;

            if (!isTriangle())
                throw new Exception("Impossible create triangle by that coords!");
        }

        public double getLine(Point p1, Point p2)
        {
            return Math.sqrt(Math.pow((p2._x - p1._x), 2) + Math.pow((p2._y - p1._y), 2));
        }

        public boolean isTriangle()
        {
            boolean is_triangle = true;

            if (AB() >= BC() + AC())
                is_triangle = false;

            if (BC() >= AB() + AC())
                is_triangle = false;

            if (AC() >= AB() + BC())
                is_triangle = false;

            return is_triangle;
        }

        public double AB() {
            return getLine(_points[0], _points[1]);
        }
        public double BC() {
            return getLine(_points[1], _points[2]);
        }
        public double AC() {
            return getLine(_points[0], _points[2]);
        }

        public double calcPerimeter() {
            return AB() + BC() + AC();
        }

        // Формула Герона
        public double calcSquare()
        {
            double p = calcPerimeter() / 2;
            return Math.sqrt(p * (p - AB()) * (p - BC()) * (p - AC()));
        }

        public Point getCenter()
        {
            return new Point((_points[0]._x + _points[1]._x + _points[2]._x) / 3,
                             (_points[0]._y + _points[1]._y + _points[2]._y) / 3);
        }

        private final Point[] _points = new Point[3];
    }

    public static void main(String[] args)
    {
        try
        {
            Triangle triangle = new Triangle(new Triangle.Point( 0, 0),
                                             new Triangle.Point( 0, 0),
                                             new Triangle.Point( 0, 0));

            System.out.printf("Perimeter = %.3f \n", triangle.calcPerimeter());
            System.out.printf("Square    = %.3f \n", triangle.calcSquare());

            Triangle.Point center = triangle.getCenter();

            System.out.printf("Center    = (%d, %d) \n", center._x, center._y);
        }
        catch (Exception e)
        {
            System.out.println("Triangle constructor trow exception " + e.toString());
        }
    }
}