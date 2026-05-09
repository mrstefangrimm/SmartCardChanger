
public class TwoLayerNetwork
{
    // Layer 1: 2 inputs → 4 hidden nodes
    private double[] weights1;
    private double[] biases1;

    // Layer 2: 4 hidden nodes → 2 outputs
    private double[] weights2;
    private double[] biases2;

    private double learningRate;
    private Random random;

    public TwoLayerNetwork(double learningRate = 0.1)
    {
        this.learningRate = learningRate;
        this.random = new Random();

        // Layer 1: 2 * 4 = 8 weights
        weights1 = new double[8];
        biases1 = new double[4];

        // Layer 2: 4 * 2 = 8 weights
        weights2 = new double[8];
        biases2 = new double[2];

        for (int i = 0; i < weights1.Length; i++)
            weights1[i] = (random.NextDouble() - 0.5) * 2;
        for (int i = 0; i < weights2.Length; i++)
            weights2[i] = (random.NextDouble() - 0.5) * 2;
    }

    private double ReLU(double x) => Math.Max(0, x);
    private double ReLUDerivative(double x) => x > 0 ? 1 : 0;

    private double[] Softmax(double[] logits)
    {
        double max = Math.Max(logits[0], logits[1]);
        double exp0 = Math.Exp(logits[0] - max);
        double exp1 = Math.Exp(logits[1] - max);
        double sum = exp0 + exp1;

        return new double[] { exp0 / sum, exp1 / sum };
    }

    public double[] Forward(double[] input)
    {
        // Layer 1: input → hidden (with ReLU)
        double[] hidden = new double[4];
        for (int j = 0; j < 4; j++)
        {
            double sum = biases1[j];
            for (int i = 0; i < 2; i++)
            {
                sum += input[i] * weights1[i * 4 + j];
            }
            hidden[j] = ReLU(sum);
        }

        // Layer 2: hidden → output (with Softmax)
        double[] logits = new double[2];
        for (int j = 0; j < 2; j++)
        {
            double sum = biases2[j];
            for (int i = 0; i < 4; i++)
            {
                sum += hidden[i] * weights2[i * 2 + j];
            }
            logits[j] = sum;
        }

        return Softmax(logits);
    }

    public void Train(double[] input, double[] expected)
    {
        // ============ FORWARD PASS ============

        // Layer 1
        double[] hidden = new double[4];
        double[] hidden_preactivation = new double[4];

        for (int j = 0; j < 4; j++)
        {
            double sum = biases1[j];
            for (int i = 0; i < 2; i++)
            {
                sum += input[i] * weights1[i * 4 + j];
            }
            hidden_preactivation[j] = sum;
            hidden[j] = ReLU(sum);
        }

        // Layer 2
        double[] logits = new double[2];
        for (int j = 0; j < 2; j++)
        {
            double sum = biases2[j];
            for (int i = 0; i < 4; i++)
            {
                sum += hidden[i] * weights2[i * 2 + j];
            }
            logits[j] = sum;
        }

        double[] output = Softmax(logits);

        // ============ LOSS CALCULATION ============
        // Cross-entropy loss: -Σ(expected[i] * log(output[i]))
        double loss = 0;
        for (int i = 0; i < 2; i++)
        {
            loss -= expected[i] * Math.Log(output[i] + 1e-10); // +1e-10 to avoid log(0)
        }

        Console.WriteLine($"Loss: {loss}");

        // ============ BACKWARD PASS ============

        // Layer 2 gradient (output layer)
        double[] delta2 = new double[2];
        for (int i = 0; i < 2; i++)
        {
            // Cross-entropy derivative
            delta2[i] = output[i] - expected[i];
        }

        // Update Layer 2 weights and biases
        for (int j = 0; j < 2; j++)
        {
            biases2[j] -= learningRate * delta2[j];
            for (int i = 0; i < 4; i++)
            {
                weights2[i * 2 + j] -= learningRate * delta2[j] * hidden[i];
            }
        }

        // Layer 1 gradient (hidden layer)
        // Error flows backward through Layer 2 weights
        double[] delta1 = new double[4];
        for (int i = 0; i < 4; i++)
        {
            double sum = 0;
            for (int j = 0; j < 2; j++)
            {
                sum += delta2[j] * weights2[i * 2 + j];
            }
            // Apply ReLU derivative
            delta1[i] = sum * ReLUDerivative(hidden_preactivation[i]);
        }

        // Update Layer 1 weights and biases
        for (int j = 0; j < 4; j++)
        {
            biases1[j] -= learningRate * delta1[j];
            for (int i = 0; i < 2; i++)
            {
                weights1[i * 4 + j] -= learningRate * delta1[j] * input[i];
            }
        }
    }

    public static void AMain()
    {
        TwoLayerNetwork nn = new TwoLayerNetwork(0.5);

        double[][] inputs = new double[][]
        {
            new double[] { 0, 0 },
            new double[] { 0, 1 },
            new double[] { 1, 0 },
            new double[] { 1, 1 }
        };

        // XOR problem - now it CAN learn!
        double[][] outputs = new double[][]
        {
            new double[] { 1, 0 }, // 0 XOR 0 = 0 (false)
            new double[] { 0, 1 }, // 0 XOR 1 = 1 (true)
            new double[] { 0, 1 }, // 1 XOR 0 = 1 (true)
            new double[] { 1, 0 }  // 1 XOR 1 = 0 (false)
        };

        // Train
        Console.WriteLine("Training...\n");
        for (int epoch = 0; epoch < 1000; epoch++)
        {
            double totalLoss = 0;
            for (int i = 0; i < inputs.Length; i++)
            {
                nn.Train(inputs[i], outputs[i]);
            }

            if (epoch % 250 == 0)
                Console.WriteLine($"Epoch {epoch}");
        }

        // Test
        Console.WriteLine("\nResults (XOR - Now it works!):");
        for (int i = 0; i < inputs.Length; i++)
        {
            double[] output = nn.Forward(inputs[i]);
            int predictedOn = output[0] > output[1] ? 0 : 1;
            int predicted = output[1] > 0.5 ? 1 : 0;
            int expected = outputs[i][1] > 0.5 ? 1 : 0;
            string match = predicted == expected ? "✓" : "✗";
            //Console.WriteLine($"Input: ({inputs[i][0]}, {inputs[i][1]}) => Class {predictedOn}, Prob: [{output[0]:F4}, {output[1]:F4}]");
            Console.WriteLine($"  {inputs[i][0]} XOR {inputs[i][1]} = {predicted} (expected {expected}) {match}");
        }
    }
}
