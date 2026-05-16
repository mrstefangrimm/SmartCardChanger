public class ClassificationTwoLayerNetworkFixed
{
    private double[] weights1;
    private double[] biases1;
    private double[] weights2;
    private double[] biases2;

    private double learningRate;
    private Random random;

    public ClassificationTwoLayerNetworkFixed(double learningRate = 0.5)
    {
        this.learningRate = learningRate;
        this.random = new Random(42);

        weights1 = new double[8];
        biases1 = new double[4];
        weights2 = new double[8];
        biases2 = new double[2];

        // Initialize weights to small random values
        for (int i = 0; i < weights1.Length; i++)
            weights1[i] = (random.NextDouble() - 0.5) * 0.5;
        for (int i = 0; i < weights2.Length; i++)
            weights2[i] = (random.NextDouble() - 0.5) * 0.5;
    }

    private double ReLU(double x) => Math.Max(0, x);
    private double ReLUDerivative(double x) => x > 0 ? 1 : 0;

    private double[] Softmax(double[] logits)
    {
        // Numerical stability: subtract max value
        double max = logits[0] > logits[1] ? logits[0] : logits[1];

        double exp0 = Math.Exp(Math.Min(logits[0] - max, 100)); // Cap to prevent overflow
        double exp1 = Math.Exp(Math.Min(logits[1] - max, 100));
        double sum = exp0 + exp1;

        if (sum == 0 || double.IsNaN(sum))
        {
            return new double[] { 0.5, 0.5 }; // Fallback
        }

        return new double[] { exp0 / sum, exp1 / sum };
    }

    public double[] Forward(double[] input)
    {
        double[] hidden = new double[4];
        for (int j = 0; j < 4; j++)
        {
            double sum = biases1[j];
            for (int i = 0; i < 2; i++)
                sum += input[i] * weights1[i * 4 + j];
            hidden[j] = ReLU(sum);
        }

        double[] logits = new double[2];
        for (int j = 0; j < 2; j++)
        {
            double sum = biases2[j];
            for (int i = 0; i < 4; i++)
                sum += hidden[i] * weights2[i * 2 + j];
            logits[j] = sum;
        }

        return Softmax(logits);
    }

    public double Train(double[] input, double[] expected)
    {
        // ============ FORWARD PASS ============
        double[] hidden = new double[4];
        double[] hidden_preactivation = new double[4];

        for (int j = 0; j < 4; j++)
        {
            double sum = biases1[j];
            for (int i = 0; i < 2; i++)
                sum += input[i] * weights1[i * 4 + j];
            hidden_preactivation[j] = sum;
            hidden[j] = ReLU(sum);
        }

        double[] logits = new double[2];
        for (int j = 0; j < 2; j++)
        {
            double sum = biases2[j];
            for (int i = 0; i < 4; i++)
                sum += hidden[i] * weights2[i * 2 + j];
            logits[j] = sum;
        }

        double[] output = Softmax(logits);

        // ============ LOSS CALCULATION ============
        double loss = 0;
        for (int i = 0; i < 2; i++)
        {
            if (output[i] > 0)
                loss -= expected[i] * Math.Log(output[i]);
        }

        // ============ BACKWARD PASS ============

        // Layer 2 gradient
        double[] delta2 = new double[2];
        for (int i = 0; i < 2; i++)
        {
            delta2[i] = output[i] - expected[i];
        }

        // Update Layer 2 with gradient clipping
        for (int j = 0; j < 2; j++)
        {
            double clipped = Math.Max(-1, Math.Min(1, delta2[j])); // Clip gradients
            biases2[j] -= learningRate * clipped;
            for (int i = 0; i < 4; i++)
            {
                weights2[i * 2 + j] -= learningRate * clipped * hidden[i];
            }
        }

        // Layer 1 gradient
        double[] delta1 = new double[4];
        for (int i = 0; i < 4; i++)
        {
            double sum = 0;
            for (int j = 0; j < 2; j++)
            {
                sum += delta2[j] * weights2[i * 2 + j];
            }
            delta1[i] = sum * ReLUDerivative(hidden_preactivation[i]);
        }

        // Update Layer 1 with gradient clipping
        for (int j = 0; j < 4; j++)
        {
            double clipped = Math.Max(-1, Math.Min(1, delta1[j]));
            biases1[j] -= learningRate * clipped;
            for (int i = 0; i < 2; i++)
            {
                weights1[i * 4 + j] -= learningRate * clipped * input[i];
            }
        }

        return loss;
    }

    public static void AMain()
    {
        var nn = new ClassificationTwoLayerNetworkFixed(learningRate: 0.5);

        double[][] inputs = new double[][]
        {
            new double[] { 0, 0 },
            new double[] { 0, 1 },
            new double[] { 1, 0 },
            new double[] { 1, 1 }
        };

        double[][] outputs = new double[][]
        {
            new double[] { 1, 0 }, // 0 XOR 0 = 0
            new double[] { 0, 1 }, // 0 XOR 1 = 1
            new double[] { 0, 1 }, // 1 XOR 0 = 1
            new double[] { 1, 0 }  // 1 XOR 1 = 0
        };

        // Train
        Console.WriteLine("Training XOR...\n");
        for (int epoch = 0; epoch < 500; epoch++)
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
        Console.WriteLine("\nResults:");
        for (int i = 0; i < inputs.Length; i++)
        {
            double[] output = nn.Forward(inputs[i]);
            int predicted = output[1] > 0.5 ? 1 : 0;
            int expected = outputs[i][1] > 0.5 ? 1 : 0;
            string match = predicted == expected ? "✓" : "✗";
            Console.WriteLine($"  {inputs[i][0]} XOR {inputs[i][1]} = {predicted} " +
                $"(expected {expected}, prob: {output[1]:F4}) {match}");
        }
    }
}
