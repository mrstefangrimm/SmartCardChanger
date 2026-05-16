public class ClassificationTwoLayerNetworkXor
{
    private readonly double _learningRate;

    // Network architecture
    private readonly int _inputSize = 2;
    private readonly int _hiddenSize = 4;
    private readonly int _outputSize = 2;

    private readonly double[][] _weightsIH; // Input to Hidden, w[h][i]
    private readonly double[][] _weightsHO; // Hidden to Output, w[o][h]
    private readonly double[] _biasesH;
    private readonly double[] _biasesO;

    public ClassificationTwoLayerNetworkXor(double learningRate = 0.01)
    {
        _learningRate = learningRate;

        var random = new Random(42); // Fixed seed for reproducibility

        // He initialization for ReLU
        _weightsIH = new double[_hiddenSize][];
        for (int ih = 0; ih < _hiddenSize; ih++)
        {
            _weightsIH[ih] = new double[_inputSize];
            double stdDev = Math.Sqrt(2.0 / _inputSize);
            for (int ii = 0; ii < _inputSize; ii++)
            {
                _weightsIH[ih][ii] = (random.NextDouble() - 0.5) * 0.1; //2 * stdDev;
            }
        }

        _weightsHO = new double[_outputSize][];
        for (int io = 0; io < _outputSize; io++)
        {
            _weightsHO[io] = new double[_hiddenSize];
            double stdDev = Math.Sqrt(2.0 / _hiddenSize);
            for (int ih = 0; ih < _hiddenSize; ih++)
            {
                _weightsHO[io][ih] = (random.NextDouble() - 0.5) * 0.1; //2 * stdDev;
            }
        }

        _biasesH = new double[_hiddenSize];
        _biasesO = new double[_outputSize];
    }

    private double ReLU(double x) => Math.Max(0, x);

    private double ReLUDerivative(double x) => x > 0 ? 1 : 0;

    private double[] Softmax(double[] z)
    {
        // Subtract max for numerical stability
        double maxZ = z.Max();
        double[] exp = z.Select(x => Math.Exp(x - maxZ)).ToArray();
        double sum = exp.Sum();
        return exp.Select(x => x / sum).ToArray();
    }

    public double[] Forward(double[] input)
    {
        var hidden = new double[_hiddenSize];

        // Input to Hidden
        for (int ih = 0; ih < _hiddenSize; ih++)
        {
            double sum = _biasesH[ih];
            for (int ii = 0; ii < _inputSize; ii++)
            {
                sum += _weightsIH[ih][ii] * input[ii];
            }
            hidden[ih] = ReLU(sum);
        }

        // Hidden to Output
        var output_preactivation = new double[_outputSize];
        for (int io = 0; io < _outputSize; io++)
        {
            output_preactivation[io] = _biasesO[io];
            for (int ih = 0; ih < _hiddenSize; ih++)
            {
                output_preactivation[io] += _weightsHO[io][ih] * hidden[ih];
            }
        }

        return Softmax(output_preactivation);
    }

    public double Train(double[] input, double[] target)
    {
        // ============ FORWARD PASS ============

        // Layer 1, Input to hidden

        var hidden_preactivation = new double[_hiddenSize];
        var hidden = new double[_hiddenSize];

        // Input to Hidden
        for (int ih = 0; ih < _hiddenSize; ih++)
        {
            hidden_preactivation[ih] = _biasesH[ih];
            for (int ii = 0; ii < _inputSize; ii++)
            {
                hidden_preactivation[ih] += _weightsIH[ih][ii] * input[ii];
            }
            hidden[ih] = ReLU(hidden_preactivation[ih]);
        }

        // Layer 2 Hidden to Output
        var output_preactivation = new double[_outputSize];
        for (int io = 0; io < _outputSize; io++)
        {
            output_preactivation[io] = _biasesO[io];
            for (int ih = 0; ih < _hiddenSize; ih++)
            {
                output_preactivation[io] += _weightsHO[io][ih] * hidden[ih];
            }
        }

        double[] output = Softmax(output_preactivation);

        // ============ LOSS CALCULATION ============

        var loss = CalculateLoss(target, output);

        // ============ BACKWARD PASS ============

        // Output layer error (Softmax + Cross-Entropy)
        double[] outputError = new double[_outputSize];
        for (int i = 0; i < _outputSize; i++)
        {
            outputError[i] = output[i] - target[i];
        }

        for (int io = 0; io < _outputSize; io++)
        {
            var clipped = Math.Max(-1, Math.Min(1, outputError[io])); // Clip gradients
            _biasesO[io] -= _learningRate * clipped;
            for (int ih = 0; ih < _hiddenSize; ih++)
            {
                _weightsHO[io][ih] -= _learningRate * clipped * hidden[ih];
            }
        }

        // Hidden layer error
        double[] hiddenError = new double[_hiddenSize];
        for (int ih = 0; ih < _hiddenSize; ih++)
        {
            hiddenError[ih] = 0;
            for (int io = 0; io < _outputSize; io++)
            {
                hiddenError[ih] += outputError[io] * _weightsHO[io][ih];
            }
            hiddenError[ih] *= ReLUDerivative(hidden_preactivation[ih]);
        }

        // Update Output layer weights
        for (int io = 0; io < _outputSize; io++)
        {
            var clipped = Math.Max(-1, Math.Min(1, outputError[io])); // Clip gradients
            _biasesO[io] -= _learningRate * clipped;
            for (int ih = 0; ih < _hiddenSize; ih++)
            {
                _weightsHO[io][ih] -= _learningRate * clipped * hidden[ih];
            }
        }

        // Update Hidden layer weights
        for (int ih = 0; ih < _hiddenSize; ih++)
        {
            var clipped = Math.Max(-1, Math.Min(1, hiddenError[ih]));
            _biasesH[ih] -= _learningRate * clipped;
            for (int ii = 0; ii < _inputSize; ii++)
            {
                _weightsIH[ih][ii] -= _learningRate * clipped * input[ii];
            }
        }

        return loss;
    }

    // Cross-entropy loss: -Σ(target[i] * log(output[i]))
    public double CalculateLoss(double[] target, double[] output)
    {
        // Cross-Entropy loss
        double loss = 0;
        for (int i = 0; i < _outputSize; i++)
        {
            if (output[i] > 0) loss -= target[i] * Math.Log(Math.Max(output[i], 1e-10));
        }
        return loss;
    }

    public int Predict(double[] input)
    {
        var output = Forward(input);
        return output.ToList().IndexOf(output.Max());
    }

    public static void AMain()
    {
        double[][] inputs =
        [
            [0, 0],
            [0, 1],
            [1, 0],
            [1, 1],
        ];
        double[][] targets =
        [
            [1, 0], // 0 XOR 0 = Closed
            [0, 1], // 0 XOR 1 = Open
            [0, 1], // 1 XOR 0 = Open
            [1, 0], // 1 XOR 1 = Closed
        ];

        // Shuffle data
        var indices = Enumerable.Range(0, inputs.Length).ToList();
        for (int i = indices.Count - 1; i > 0; i--)
        {
            int randomIndex = new Random().Next(i + 1);
            var temp = indices[i];
            indices[i] = indices[randomIndex];
            indices[randomIndex] = temp;
        }

        var shuffledInputs = indices.Select(i => inputs[i]).ToArray();
        var shuffledTargets = indices.Select(i => targets[i]).ToArray();

        var trainInputs = shuffledInputs;
        var trainTargets = shuffledTargets;
        var testInputs = trainInputs;
        var testTargets = trainTargets;

        // Train network
        var nn = new ClassificationTwoLayerNetworkXor(0.01);
        int epochs = 10000;

        Console.WriteLine("Training...");

        int patienceCounter = 0;
        int patience = 100; // Stop if no improvement for 500 epochs
        double bestLoss = double.MaxValue;

        for (int epoch = 0; epoch < epochs; epoch++)
        {
            ShuffleData(trainInputs, trainTargets);

            double totalLoss = 0;
            for (int i = 0; i < trainInputs.Length; i++)
            {
                totalLoss += nn.Train(trainInputs[i], trainTargets[i]);
            }
            totalLoss /= trainInputs.Length;

            if (totalLoss < bestLoss)
            {
                bestLoss = totalLoss;
                patienceCounter = 0; // Reset counter
            }
            else
            {
                patienceCounter++;
            }

            if (epoch % 200 == 0)
            {
                 Console.WriteLine($"Epoch {epoch}: Loss = {totalLoss:F6} (patience: {patienceCounter})");

                // Stop if no improvement
                if (patienceCounter >= patience)
                {
                    Console.WriteLine($"Stopped at epoch {epoch}. No improvement for {patience} epochs.");
                    break;
                }
            }
        }

        // Test network
        Console.WriteLine();
        Console.WriteLine("Testing...");
        int correct = 0;
        for (int i = 0; i < testInputs.Length; i++)
        {
            int prediction = nn.Predict(testInputs[i]);
            int actual = testTargets[i].ToList().IndexOf(1.0);
            if (prediction == actual)
                correct++;

            Console.WriteLine($"Sample {i}: Predicted {GetClassName(prediction)}, Output {GetClassName(actual)}");
        }

        double accuracy = (double)correct / testInputs.Length * 100;
        Console.WriteLine($"\nAccuracy: {accuracy:F2}%");
    }

    private static void ShuffleData(double[][] inputs, double[][] targets)
    {
        var random = new Random();
        for (int i = inputs.Length - 1; i > 0; i--)
        {
            int randomIndex = random.Next(i + 1);

            // Swap inputs
            var tempInput = inputs[i];
            inputs[i] = inputs[randomIndex];
            inputs[randomIndex] = tempInput;

            // Swap corresponding targets
            var tempTarget = targets[i];
            targets[i] = targets[randomIndex];
            targets[randomIndex] = tempTarget;
        }
    }

    private static string GetClassName(int index) => index switch
    {
        0 => "Closed",
        1 => "Open",
        _ => throw new IndexOutOfRangeException()
    };

}