public class RegressionSimpleNetworkScc
{
    private double learningRate = 0.001;  // Reduced from 0.01

    private int _inputSize = 1;
    private int _hiddenSize = 16;
    private int _outputSize = 1;

    private double[][] _weightsIH;  // input -> hidden
    private double[][] _weightsHO;  // hidden -> output
    private double[] _biasesH;
    private double[] _biasesO;

    private Random random = new Random(42);

    public RegressionSimpleNetworkScc()
    {
        InitializeWeights();
    }

    private void InitializeWeights()
    {
        // Xavier initialization (better than random)
        double limit1 = Math.Sqrt(6.0 / (_inputSize + _hiddenSize));
        double limit2 = Math.Sqrt(6.0 / (_hiddenSize + _outputSize));

        _weightsIH = new double[_inputSize][];
        for (int ii = 0; ii < _inputSize; ii++)
        {
            _weightsIH[ii] = new double[_hiddenSize];
            for (int ih = 0; ih < _hiddenSize; ih++)
            {
                _weightsIH[ii][ih] = (random.NextDouble() - 0.5) * 2 * limit1;
            }
        }

        _weightsHO = new double[_hiddenSize][];
        for (int ih = 0; ih < _hiddenSize; ih++)
        {
            _weightsHO[ih] = new double[_outputSize];
            for (int io = 0; io < _outputSize; io++)
            {
                _weightsHO[ih][io] = (random.NextDouble() - 0.5) * 2 * limit2;
            }
        }

        _biasesH = new double[_hiddenSize];
        _biasesO = new double[_outputSize];
    }

    private double ReLU(double x) => Math.Max(0, x);
    private double ReLUDerivative(double x) => x > 0 ? 1 : 0;

    public double Predict(double input)
    {
        // Forward pass
        double[] hiddenInput = new double[_hiddenSize];
        double[] hiddenOutput = new double[_hiddenSize];

        for (int ih = 0; ih < _hiddenSize; ih++)
        {
            hiddenInput[ih] = _weightsIH[0][ih] * input + _biasesH[ih];
            hiddenOutput[ih] = ReLU(hiddenInput[ih]);
        }

        double output = _biasesO[0];
        for (int io = 0; io < _hiddenSize; io++)
        {
            output += _weightsHO[io][0] * hiddenOutput[io];
        }

        return output;
    }

    public void Train(double[] inputs, double[] targets, int epochs)
    {
        // Check for NaN in input data
        if (inputs.Any(x => double.IsNaN(x) || double.IsInfinity(x)))
        {
            Console.WriteLine("ERROR: Input data contains NaN or Infinity!");
            return;
        }
        if (targets.Any(x => double.IsNaN(x) || double.IsInfinity(x)))
        {
            Console.WriteLine("ERROR: Target data contains NaN or Infinity!");
            return;
        }

        Console.WriteLine($"Training data size: {inputs.Length}");
        Console.WriteLine($"X range: [{inputs.Min():F4}, {inputs.Max():F4}]");
        Console.WriteLine($"Alpha range: [{targets.Min():F4}, {targets.Max():F4}]");

        for (int epoch = 0; epoch < epochs; epoch++)
        {
            double totalError = 0;
            int validSamples = 0;

            for (int sampleIdx = 0; sampleIdx < inputs.Length; sampleIdx++)
            {
                double input = inputs[sampleIdx];
                double target = targets[sampleIdx];

                // Skip invalid data
                if (double.IsNaN(input) || double.IsNaN(target) ||
                    double.IsInfinity(input) || double.IsInfinity(target))
                {
                    continue;
                }

                validSamples++;

                // Forward pass
                double[] hiddenInput = new double[_hiddenSize];
                double[] hiddenOutput = new double[_hiddenSize];

                for (int j = 0; j < _hiddenSize; j++)
                {
                    hiddenInput[j] = _weightsIH[0][j] * input + _biasesH[j];
                    hiddenOutput[j] = ReLU(hiddenInput[j]);
                }

                double prediction = _biasesO[0];
                for (int j = 0; j < _hiddenSize; j++)
                {
                    prediction += _weightsHO[j][0] * hiddenOutput[j];
                }

                // Compute error
                double error = target - prediction;

                // Check for NaN
                if (double.IsNaN(error) || double.IsInfinity(error))
                {
                    Console.WriteLine($"NaN detected at sample {sampleIdx}: input={input}, target={target}, prediction={prediction}");
                    continue;
                }

                totalError += error * error;

                // Backpropagation
                double outputGradient = -2 * error;

                // Update layer 2
                for (int j = 0; j < _hiddenSize; j++)
                {
                    double gradient = outputGradient * hiddenOutput[j];
                    _weightsHO[j][0] -= learningRate * gradient;
                }
                _biasesO[0] -= learningRate * outputGradient;

                // Update layer 1
                double[] hiddenGradients = new double[_hiddenSize];
                for (int j = 0; j < _hiddenSize; j++)
                {
                    hiddenGradients[j] = outputGradient * _weightsHO[j][0] * ReLUDerivative(hiddenInput[j]);
                }

                for (int j = 0; j < _hiddenSize; j++)
                {
                    double gradient = hiddenGradients[j] * input;
                    _weightsIH[0][j] -= learningRate * gradient;
                    _biasesH[j] -= learningRate * hiddenGradients[j];
                }
            }

            double mse = validSamples > 0 ? totalError / validSamples : double.NaN;

            if (epoch % 100 == 0)
            {
                Console.WriteLine($"Epoch {epoch}: MSE = {mse:F6}, Valid samples = {validSamples}");
            }

            // Detect divergence
            if (double.IsNaN(mse) || mse > 1e6)
            {
                Console.WriteLine("ERROR: Training diverged! Reduce learning rate or check data.");
                break;
            }
        }
    }

    public static void AMain()
    {
        // Generate training data
        var xValues = new List<double>();
        var alphaValues = new List<double>();

        double a = 40, b = 175, d = 0;

        Console.WriteLine("Generating training data...");

        for (double alpha = 0; alpha < Math.PI; alpha += 0.02)
        {
            double x = Math.Sqrt(b * b - Math.Pow(a * Math.Sin(alpha) - d, 2)) - a * Math.Cos(alpha);

            xValues.Add(x);
            alphaValues.Add(alpha);
        }

        Console.WriteLine($"Generated {xValues.Count} training samples");

        if (xValues.Count < 10)
        {
            Console.WriteLine("ERROR: Not enough valid training data!");
            return;
        }

        //xValues =
        //    [
        //    0,
        //    10,
        //    20,
        //    40,
        //    60,
        //    70
        //    ];

        //alphaValues =
        //    [
        //    950,
        //    1090,
        //    1180,
        //    1330,
        //    1505,
        //    1630
        //    ];

        // Normalize data
        double xMin = xValues.Min();
        double xMax = xValues.Max();
        double alphaMin = alphaValues.Min();
        double alphaMax = alphaValues.Max();

        double[] normalizedX = xValues.Select(x => (x - xMin) / (xMax - xMin)).ToArray();
        double[] normalizedAlpha = alphaValues.Select(a => (a - alphaMin) / (alphaMax - alphaMin)).ToArray();

        Console.WriteLine($"X normalized range: [{normalizedX.Min():F4}, {normalizedX.Max():F4}]");
        Console.WriteLine($"Alpha normalized range: [{normalizedAlpha.Min():F4}, {normalizedAlpha.Max():F4}]");

        // Train network
        var network = new RegressionSimpleNetworkScc();
        network.Train(normalizedX, normalizedAlpha, 5000);

        // Test
        Console.WriteLine("\n--- Testing ---");
        double testAlpha = Math.PI;
        double testX = Math.Sqrt(b * b - Math.Pow(a * Math.Sin(testAlpha) - d, 2)) - a * Math.Cos(testAlpha);

        //double testAlpha = 1180;
        //double testX = 20;

        double normalizedTestX = (testX - xMin) / (xMax - xMin);
        double predicted = network.Predict(normalizedTestX);
        double denormalizedPredicted = predicted * (alphaMax - alphaMin) + alphaMin;

        Console.WriteLine($"Test X: {testX:F4}");
        Console.WriteLine($"Actual Alpha: {testAlpha:F4}");
        Console.WriteLine($"Predicted Alpha: {denormalizedPredicted:F4}");
        Console.WriteLine($"Error: {Math.Abs(testAlpha - denormalizedPredicted):F4}");

        for (int i = 0; i < 70; i++)
        {
            double normalizedI = (i - xMin) / (xMax - xMin);
            double predictedA = network.Predict(normalizedI);
            double denormalizedPredictedA = predictedA * (alphaMax - alphaMin) + alphaMin;

            double sinAlpha = Math.Sin(denormalizedPredictedA);
            double cosAlpha = Math.Cos(denormalizedPredictedA);
            double innerTerm = a * sinAlpha - d;
            double discriminant = b * b - innerTerm * innerTerm;

            double x = Math.Sqrt(discriminant) - a * cosAlpha;


            Console.WriteLine($"{i}, {(x - 135.3792):F4}, {denormalizedPredictedA:F4}");
        }
    }
}
