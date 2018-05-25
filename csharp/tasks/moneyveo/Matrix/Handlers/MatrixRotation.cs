﻿namespace Matrix.Handlers
{
    public class MatrixRotation<T> : IMatrixHandler
    {
        private readonly bool _clockwise;

        public MatrixRotation(bool clockwise = true)
        {
            _clockwise = clockwise;
        }

        public Matrix For(Matrix matrix)
        {
            return For((Matrix<T>)matrix);
        }

        private Matrix For(Matrix<T> matrix)
        {
            T val;

            uint n = matrix.Width;

            for (uint x = 0, y = 0; x < n >> 1; x++, y++)
            {
                val = matrix[x, y];
            }

            return matrix;
        }
    }
}