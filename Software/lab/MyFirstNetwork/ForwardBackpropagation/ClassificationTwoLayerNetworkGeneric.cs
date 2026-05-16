public class ClassificationTwoLayerNetworkGeneric
{
    private const int _inputSize = 2;
    private const int _hiddenSize = 4;
    private const int _outputSize = 2;

    private readonly double[] _weightsIH;
    private readonly double[] _weightsHO;
    private readonly double[] _biasesH;
    private readonly double[] _biasesO;

    private readonly double _learningRate;

    public ClassificationTwoLayerNetworkGeneric(double learningRate = 0.5)
    {
        _learningRate = learningRate;
        var random = new Random(42);

        // Layer 1: InputNodes * HiddenNodes = num weights
        _weightsIH = new double[_inputSize * _hiddenSize];
        _biasesH = new double[_hiddenSize];


        // Layer 2: HiddenNodes * OutputNodes = num weights
        _weightsHO = new double[_hiddenSize * _outputSize];
        _biasesO = new double[_outputSize];

        // Initialize weights to small random values
        for (int i = 0; i < _weightsIH.Length; i++) _weightsIH[i] = (random.NextDouble() - 0.5) * 0.5;
        for (int i = 0; i < _weightsHO.Length; i++) _weightsHO[i] = (random.NextDouble() - 0.5) * 0.5;
    }

    private static double ReLU(double x) => Math.Max(0, x);
    private static double ReLUDerivative(double x) => x > 0 ? 1 : 0;

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
        // Layer 1: input → hidden (with ReLU)
        var hidden = new double[_hiddenSize];
        for (int ih = 0; ih < _hiddenSize; ih++)
        {
            double sum = _biasesH[ih];
            for (int ii = 0; ii < _inputSize; ii++)
            {
                sum += input[ii] * _weightsIH[ii * _hiddenSize + ih];
            }
            hidden[ih] = ReLU(sum);
        }

        // Layer 2: hidden → output (with Softmax)
        var output_preactivation = new double[_outputSize];
        for (int io = 0; io < _outputSize; io++)
        {
            output_preactivation[io] = _biasesO[io];
            for (int ih = 0; ih < _hiddenSize; ih++)
            {
                output_preactivation[io] += hidden[ih] * _weightsHO[ih * _outputSize + io];
            }
        }

        return Softmax(output_preactivation);
    }

    public double Train(double[] input, double[] target)
    {
        // ============ FORWARD PASS ============

        // Layer 1
        var hidden_preactivation = new double[_hiddenSize];
        var hidden = new double[_hiddenSize];

        for (int ih = 0; ih < _hiddenSize; ih++)
        {
            hidden_preactivation[ih] = _biasesH[ih];
            for (int ii = 0; ii < _inputSize; ii++)
            {
                hidden_preactivation[ih] += input[ii] * _weightsIH[ii * _hiddenSize + ih];
            }
            hidden[ih] = ReLU(hidden_preactivation[ih]);
        }

        // Layer 2
        var output_preactivation = new double[_outputSize];
        for (int io = 0; io < _outputSize; io++)
        {
            output_preactivation[io] = _biasesO[io];
            for (int ih = 0; ih < _hiddenSize; ih++)
            {
                output_preactivation[io] += hidden[ih] * _weightsHO[ih * _outputSize + io];
            }
        }

        var output = Softmax(output_preactivation);

        // ============ LOSS CALCULATION ============
        var loss = CalculateLoss(target, output);

        // ============ BACKWARD PASS ============

        // Layer 2 gradient (output layer)
        double[] outputError = new double[_outputSize];
        for (int i = 0; i < _outputSize; i++)
        {
            // Cross-entropy derivative
            outputError[i] = output[i] - target[i];
        }

        // Update Layer 2 weights and biases
        // Update Layer 2 with gradient clipping
        for (int io = 0; io < _outputSize; io++)
        {
            var clipped = Math.Max(-1, Math.Min(1, outputError[io])); // Clip gradients
            _biasesO[io] -= _learningRate * clipped;
            for (int ih = 0; ih < _hiddenSize; ih++)
            {
                _weightsHO[ih * _outputSize + io] -= _learningRate * clipped * hidden[ih];
            }
        }

        // Layer 1 gradient (hidden layer)
        // Error flows backward through Layer 2 weights
        // Layer 1 gradient
        double[] hiddenError = new double[_hiddenSize];
        for (int ih = 0; ih < _hiddenSize; ih++)
        {
            hiddenError[ih] = 0;
            for (int io = 0; io < _outputSize; io++)
            {
                hiddenError[ih] += outputError[io] * _weightsHO[ih * _outputSize + io];
            }
            // Apply ReLU derivative
            hiddenError[ih] *= ReLUDerivative(hidden_preactivation[ih]);
        }

        // Update Output layer weights
        for (int io = 0; io < _outputSize; io++)
        {
            var clipped = Math.Max(-1, Math.Min(1, outputError[io])); // Clip gradients
            _biasesO[io] -= _learningRate * outputError[io];
            for (int ih = 0; ih < _hiddenSize; ih++)
            {
                _weightsHO[ih * _outputSize + io] -= _learningRate * outputError[io] * hidden[ih];
            }
        }

        // Update Layer 1 weights and biases
        // Update Layer 1 with gradient clipping
        for (int ih = 0; ih < _hiddenSize; ih++)
        {
            var clipped = Math.Max(-1, Math.Min(1, hiddenError[ih]));
            _biasesH[ih] -= _learningRate * clipped;
            for (int ii = 0; ii < _inputSize; ii++)
            {
                _weightsIH[ii * _hiddenSize + ih] -= _learningRate * clipped * input[ii];
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

    public static void AMain()
    {
        var nn = new ClassificationTwoLayerNetworkGeneric(learningRate: 0.5);

        double[][] inputs =
        [
            [0, 0],
            [0, 1],
            [1, 0],
            [1, 1],
        ];

        // Categories: index 0 == false, index 1 == true
        double[][] outputs =
        [
            [1, 0], // 0 XOR 0 = 0
            [0, 1], // 0 XOR 1 = 1
            [0, 1], // 1 XOR 0 = 1
            [1, 0], // 1 XOR 1 = 0
        ];

        // Train
        Console.WriteLine("Training XOR...");
        Console.WriteLine();
        for (int epoch = 0; epoch < 10000; epoch++)
        {
            double totalLoss = 0;
            for (int i = 0; i < inputs.Length; i++)
            {
                totalLoss += nn.Train(inputs[i], outputs[i]);
            }

            if (epoch % 500 == 0)
                Console.WriteLine($"Epoch {epoch}: Loss = {totalLoss:F6}");
        }

        // Test
        Console.WriteLine();
        Console.WriteLine("Results:");
        for (int i = 0; i < inputs.Length; i++)
        {
            double[] output = nn.Forward(inputs[i]);
            int predicted = output[1] > 0.5 ? 1 : 0;
            int expected = outputs[i][1] > 0.5 ? 1 : 0;
            string match = predicted == expected ? "✓" : "✗";
            Console.WriteLine($"  {inputs[i][0]} XOR {inputs[i][1]} = {predicted} (expected {expected}, prob: {output[1]:F4}) {match}");
        }
    }
}
