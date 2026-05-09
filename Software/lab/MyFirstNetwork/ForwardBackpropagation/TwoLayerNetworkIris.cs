public class TwoLayerNetworkIris
{
    private readonly double _learningRate;

    // Network architecture
    private readonly int _inputSize = 4;
    private readonly int _hiddenSize = 3;
    private readonly int _outputSize = 3;

    private readonly double[][] _weightsIH; // Input to Hidden, w[h][i]
    private readonly double[][] _weightsHO; // Hidden to Output, w[o][h]
    private readonly double[] _biasesH;
    private readonly double[] _biasesO;

    public TwoLayerNetworkIris(double learningRate = 0.01)
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
                _weightsIH[ih][ii] = (random.NextDouble() - 0.5) * 2 * stdDev;
            }
        }

        _weightsHO = new double[_outputSize][];
        for (int io = 0; io < _outputSize; io++)
        {
            _weightsHO[io] = new double[_hiddenSize];
            double stdDev = Math.Sqrt(2.0 / _hiddenSize);
            for (int ih = 0; ih < _hiddenSize; ih++)
            {
                _weightsHO[io][ih] = (random.NextDouble() - 0.5) * 2 * stdDev;
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
        // Load Iris dataset
        var data = LoadIrisDataset();
        var inputs = data.inputs;
        var targets = data.targets;

        // Normalize inputs
        NormalizeInputs(inputs);

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

        // Split into train/test (80/20)
        int trainSize = (int)(shuffledInputs.Length * 0.8);
        var trainInputs = shuffledInputs.Take(trainSize).ToArray();
        var trainTargets = shuffledTargets.Take(trainSize).ToArray();
        var testInputs = shuffledInputs.Skip(trainSize).ToArray();
        var testTargets = shuffledTargets.Skip(trainSize).ToArray();

        // Train network
        var nn = new TwoLayerNetworkIris();
        int epochs = 2000;

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
        0 => "Iris-setosa",
        1 => "Iris-versicolor",
        2 => "Iris-virginica",
        _ => throw new IndexOutOfRangeException()
    };

    private static void NormalizeInputs(double[][] inputs)
    {
        int numFeatures = inputs[0].Length;
        double[] mins = new double[numFeatures];
        double[] maxs = new double[numFeatures];

        for (int j = 0; j < numFeatures; j++)
        {
            mins[j] = inputs.Min(row => row[j]);
            maxs[j] = inputs.Max(row => row[j]);
        }

        for (int i = 0; i < inputs.Length; i++)
        {
            for (int j = 0; j < numFeatures; j++)
            {
                inputs[i][j] = (inputs[i][j] - mins[j]) / (maxs[j] - mins[j]);
            }
        }
    }

    private static (double[][] inputs, double[][] targets) LoadIrisDataset()
    {
        // Iris dataset: 150 samples, 4 features, 3 classes
        double[][] inputs =
        [
            [5.1,3.5,1.4,0.2],
            [4.9,3.0,1.4,0.2],
            [4.9,3.0,1.4,0.2],
            [4.7,3.2,1.3,0.2],
            [4.6,3.1,1.5,0.2],
            [5.0,3.6,1.4,0.2],
            [5.4,3.9,1.7,0.4],
            [4.6,3.4,1.4,0.3],
            [5.0,3.4,1.5,0.2],
            [4.4,2.9,1.4,0.2],
            [4.9,3.1,1.5,0.1],
            [5.4,3.7,1.5,0.2],
            [4.8,3.4,1.6,0.2],
            [4.8,3.0,1.4,0.1],
            [4.3,3.0,1.1,0.1],
            [5.8,4.0,1.2,0.2],
            [5.7,4.4,1.5,0.4],
            [5.4,3.9,1.3,0.4],
            [5.1,3.5,1.4,0.3],
            [5.7,3.8,1.7,0.3],
            [5.1,3.8,1.5,0.3],
            [5.4,3.4,1.7,0.2],
            [5.1,3.7,1.5,0.4],
            [4.6,3.6,1.0,0.2],
            [5.1,3.3,1.7,0.5],
            [4.8,3.4,1.9,0.2],
            [5.0,3.0,1.6,0.2],
            [5.0,3.4,1.6,0.4],
            [5.2,3.5,1.5,0.2],
            [5.2,3.4,1.4,0.2],
            [4.7,3.2,1.6,0.2],
            [4.8,3.1,1.6,0.2],
            [5.4,3.4,1.5,0.4],
            [5.2,4.1,1.5,0.1],
            [5.5,4.2,1.4,0.2],
            [4.9,3.1,1.5,0.1],
            [5.0,3.2,1.2,0.2],
            [5.5,3.5,1.3,0.2],
            [4.9,3.1,1.5,0.1],
            [4.4,3.0,1.3,0.2],
            [5.1,3.4,1.5,0.2],
            [5.0,3.5,1.3,0.3],
            [4.5,2.3,1.3,0.3],
            [4.4,3.2,1.3,0.2],
            [5.0,3.5,1.6,0.6],
            [5.1,3.8,1.9,0.4],
            [4.8,3.0,1.4,0.3],
            [5.1,3.8,1.6,0.2],
            [4.6,3.2,1.4,0.2],
            [5.3, 3.7, 1.5, 0.2],
            [5.0, 3.3, 1.4, 0.2],

            [7.0,3.2,4.7,1.4],
            [6.4,3.2,4.5,1.5],
            [6.9,3.1,4.9,1.5],
            [5.5,2.3,4.0,1.3],
            [6.5,2.8,4.6,1.5],
            [5.7,2.8,4.5,1.3],
            [6.3,3.3,4.7,1.6],
            [4.9,2.4,3.3,1.0],
            [6.6,2.9,4.6,1.3],
            [5.2,2.7,3.9,1.4],
            [5.0,2.0,3.5,1.0],
            [5.9,3.0,4.2,1.5],
            [6.0,2.2,4.0,1.0],
            [6.1,2.9,4.7,1.4],
            [5.6,2.9,3.6,1.3],
            [6.7,3.1,4.4,1.4],
            [5.6,3.0,4.5,1.5],
            [5.8,2.7,4.1,1.0],
            [6.2,2.2,4.5,1.5],
            [5.6,2.5,3.9,1.1],
            [5.9,3.2,4.8,1.8],
            [6.1,2.8,4.0,1.3],
            [6.3,2.5,4.9,1.5],
            [6.1,2.8,4.7,1.2],
            [6.4,2.9,4.3,1.3],
            [6.6,3.0,4.4,1.4],
            [6.8,2.8,4.8,1.4],
            [6.7,3.0,5.0,1.7],
            [6.0,2.9,4.5,1.5],
            [5.7,2.6,3.5,1.0],
            [5.5,2.4,3.8,1.1],
            [5.5,2.4,3.7,1.0],
            [5.8,2.7,3.9,1.2],
            [6.0,2.7,5.1,1.6],
            [5.4,3.0,4.5,1.5],
            [6.0,3.4,4.5,1.6],
            [6.7,3.1,4.7,1.5],
            [6.3,2.3,4.4,1.3],
            [5.6,3.0,4.1,1.3],
            [5.5,2.5,4.0,1.3],
            [5.5,2.6,4.4,1.2],
            [6.1,3.0,4.6,1.4],
            [5.8,2.6,4.0,1.2],
            [5.0,2.3,3.3,1.0],
            [5.6,2.7,4.2,1.3],
            [5.7,3.0,4.2,1.2],
            [5.7,2.9,4.2,1.3],
            [6.2,2.9,4.3,1.3],
            [5.1, 2.5, 3.0, 1.1],
            [5.7, 2.8, 4.1, 1.3],

            [6.3,3.3,6.0,2.5],
            [5.8,2.7,5.1,1.9],
            [7.1,3.0,5.9,2.1],
            [6.3,2.9,5.6,1.8],
            [6.5,3.0,5.8,2.2],
            [7.6,3.0,6.6,2.1],
            [4.9,2.5,4.5,1.7],
            [7.3,2.9,6.3,1.8],
            [6.7,2.5,5.8,1.8],
            [7.2,3.6,6.1,2.5],
            [6.5,3.2,5.1,2.0],
            [6.4,2.7,5.3,1.9],
            [6.8,3.0,5.5,2.1],
            [5.7,2.5,5.0,2.0],
            [5.8,2.8,5.1,2.4],
            [6.4,3.2,5.3,2.3],
            [6.5,3.0,5.5,1.8],
            [7.7,3.8,6.7,2.2],
            [7.7,2.6,6.9,2.3],
            [6.0,2.2,5.0,1.5],
            [6.9,3.2,5.7,2.3],
            [5.6,2.8,4.9,2.0],
            [7.7,2.8,6.7,2.0],
            [6.3,2.7,4.9,1.8],
            [6.7,3.3,5.7,2.1],
            [7.2,3.2,6.0,1.8],
            [6.2,2.8,4.8,1.8],
            [6.1,3.0,4.9,1.8],
            [6.4,2.8,5.6,2.1],
            [7.2,3.0,5.8,1.6],
            [7.4,2.8,6.1,1.9],
            [7.9,3.8,6.4,2.0],
            [6.4,2.8,5.6,2.2],
            [6.3,2.8,5.1,1.5],
            [6.1,2.6,5.6,1.4],
            [7.7,3.0,6.1,2.3],
            [6.3,3.4,5.6,2.4],
            [6.4,3.1,5.5,1.8],
            [6.0,3.0,4.8,1.8],
            [6.9,3.1,5.4,2.1],
            [6.7,3.1,5.6,2.4],
            [6.9,3.1,5.1,2.3],
            [5.8,2.7,5.1,1.9],
            [6.8,3.2,5.9,2.3],
            [6.7,3.3,5.7,2.5],
            [6.7,3.0,5.2,2.3],
            [6.3,2.5,5.0,1.9],
            [6.5,3.0,5.2,2.0],
            [6.2, 3.4, 5.4, 2.3],
            [5.9, 3.0, 5.1, 1.8],
        ];

        double[][] targets =
        [
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa
            [1, 0, 0], // Iris-setosa

            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor
            [0, 1, 0], // Iris-versicolor

            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
            [0, 0, 1], // Iris-virginica
        ];

        return (inputs, targets);
    }
}