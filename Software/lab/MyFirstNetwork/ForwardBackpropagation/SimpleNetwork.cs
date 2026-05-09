public class SimpleNetwork
{
    private double[] weights;
    private double[] biases;
    private double learningRate;
    private Random random;

    public SimpleNetwork(double learningRate = 0.1)
    {
        this.learningRate = learningRate;
        this.random = new Random();

        // 2 inputs * 2 outputs = 4 weights
        weights = new double[4];
        biases = new double[2];

        // Random initialization
        for (int i = 0; i < weights.Length; i++)
        {
            weights[i] = (random.NextDouble() - 0.5) * 2;
        }
        for (int i = 0; i < biases.Length; i++)
        {
            biases[i] = 0;
        }
    }

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
        // Linear layer: output = input * weights + bias
        double logit0 = input[0] * weights[0] + input[1] * weights[1] + biases[0];
        double logit1 = input[0] * weights[2] + input[1] * weights[3] + biases[1];

        // Softmax
        return Softmax(new double[] { logit0, logit1 });
    }

    public void Train(double[] input, double[] expected)
    {
        // Forward pass
        double logit0 = input[0] * weights[0] + input[1] * weights[1] + biases[0];
        double logit1 = input[0] * weights[2] + input[1] * weights[3] + biases[1];

        double[] output = Softmax(new double[] { logit0, logit1 });

        // Backward pass - cross-entropy gradient
        double delta0 = output[0] - expected[0];
        double delta1 = output[1] - expected[1];

        // Update weights and biases
        weights[0] -= learningRate * delta0 * input[0];
        weights[1] -= learningRate * delta0 * input[1];
        weights[2] -= learningRate * delta1 * input[0];
        weights[3] -= learningRate * delta1 * input[1];

        biases[0] -= learningRate * delta0;
        biases[1] -= learningRate * delta1;
    }

    public static void AMain()
    {
        SimpleNetwork nn = new SimpleNetwork(0.5);

        double[][] inputs = new double[][]
        {
            // XOR
            //new double[] { 0, 0 },
            //new double[] { 0, 1 },
            //new double[] { 1, 0 },
            //new double[] { 1, 1 }

            // OR
            new double[] { 0, 0 },
            new double[] { 0, 1 },
            new double[] { 1, 0 },
            new double[] { 1, 1 }
        };

        double[][] outputs = new double[][]
        {
            // XOR
            //new double[] { 1, 0 }, // Class 0
            //new double[] { 0, 1 }, // Class 1
            //new double[] { 0, 1 }, // Class 1
            //new double[] { 1, 0 }  // Class 0

            // OR
             new double[] { 1, 0 }, // 0 OR 0 = 0 (false)
            new double[] { 0, 1 }, // 0 OR 1 = 1 (true)
            new double[] { 0, 1 }, // 1 OR 0 = 1 (true)
            new double[] { 0, 1 }  // 1 OR 1 = 1 (true)
        };

        // Train
        for (int epoch = 0; epoch < 10; epoch++)
        {
            for (int i = 0; i < inputs.Length; i++)
            {
                nn.Train(inputs[i], outputs[i]);
            }
        }

        // Test
        Console.WriteLine("Results:");
        for (int i = 0; i < inputs.Length; i++)
        {
            double[] output = nn.Forward(inputs[i]);
            int predicted = output[0] > output[1] ? 0 : 1;
            Console.WriteLine($"Input: ({inputs[i][0]}, {inputs[i][1]}) => " +
                $"Class {predicted}, Prob: [{output[0]:F4}, {output[1]:F4}]");
        }
    }
}
