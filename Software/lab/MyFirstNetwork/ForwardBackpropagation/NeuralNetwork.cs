
public class NeuralNetwork
{
    private double[][] weights;
    private double[] biases;
    private double learningRate;
    private Random random;

    public NeuralNetwork(int[] layerSizes, double learningRate = 0.1)
    {
        this.learningRate = learningRate;
        this.random = new Random();
        InitializeWeights(layerSizes);
    }

    private void InitializeWeights(int[] layerSizes)
    {
        weights = new double[layerSizes.Length - 1][];
        biases = new double[layerSizes.Length - 1];

        for (int i = 0; i < layerSizes.Length - 1; i++)
        {
            weights[i] = new double[layerSizes[i] * layerSizes[i + 1]];

            // Initialize weights randomly
            for (int j = 0; j < weights[i].Length; j++)
            {
                weights[i][j] = (random.NextDouble() - 0.5) * 2;
            }
            biases[i] = (random.NextDouble() - 0.5) * 2;
        }
    }

    private double Sigmoid(double x)
    {
        return 1.0 / (1.0 + Math.Exp(-x));
    }

    private double SigmoidDerivative(double output)
    {
        return output * (1.0 - output);
    }

    public double[] Forward(double[] input)
    {
        double[] current = input;

        for (int layer = 0; layer < weights.Length; layer++)
        {
            int inputSize = (layer == 0) ? input.Length : current.Length;
            int outputSize = (layer == weights.Length - 1) ?
                GetOutputSize(layer) : GetOutputSize(layer);

            double[] next = new double[outputSize];

            for (int j = 0; j < outputSize; j++)
            {
                double sum = biases[layer];
                for (int i = 0; i < inputSize; i++)
                {
                    sum += current[i] * weights[layer][i * outputSize + j];
                }
                next[j] = Sigmoid(sum);
            }
            current = next;
        }

        return current;
    }

    public void Backpropagate(double[] input, double[] expectedOutput)
    {
        // Forward pass (store activations for backward pass)
        double[][] activations = new double[weights.Length + 1][];
        activations[0] = input;
        double[] current = input;

        for (int layer = 0; layer < weights.Length; layer++)
        {
            int outputSize = GetOutputSize(layer);
            double[] next = new double[outputSize];

            for (int j = 0; j < outputSize; j++)
            {
                double sum = biases[layer];
                for (int i = 0; i < current.Length; i++)
                {
                    sum += current[i] * weights[layer][i * outputSize + j];
                }
                next[j] = Sigmoid(sum);
            }
            activations[layer + 1] = next;
            current = next;
        }

        // Backward pass
        double[] deltas = new double[GetOutputSize(weights.Length - 1)];
        for (int i = 0; i < deltas.Length; i++)
        {
            double output = activations[weights.Length][i];
            deltas[i] = (expectedOutput[i] - output) * SigmoidDerivative(output);
        }

        // Update weights and biases
        for (int layer = weights.Length - 1; layer >= 0; layer--)
        {
            double[] layerActivations = activations[layer];
            int outputSize = GetOutputSize(layer);

            for (int j = 0; j < outputSize; j++)
            {
                biases[layer] += learningRate * deltas[j];

                for (int i = 0; i < layerActivations.Length; i++)
                {
                    int index = i * outputSize + j;
                    weights[layer][index] += learningRate * deltas[j] * layerActivations[i];
                }
            }

            // Calculate deltas for previous layer
            if (layer > 0)
            {
                double[] prevDeltas = new double[layerActivations.Length];
                int currentOutputSize = GetOutputSize(layer);

                for (int i = 0; i < prevDeltas.Length; i++)
                {
                    double sum = 0;
                    for (int j = 0; j < currentOutputSize; j++)
                    {
                        sum += deltas[j] * weights[layer][i * currentOutputSize + j];
                    }
                    prevDeltas[i] = sum * SigmoidDerivative(activations[layer][i]);
                }
                deltas = prevDeltas;
            }
        }
    }

    private int GetOutputSize(int layerIndex)
    {
        // This is a simplified version; you'd need to track layer sizes properly
        return weights[layerIndex].Length / (layerIndex == 0 ? 2 : weights[layerIndex - 1].Length);
    }

    public static void AMain()
    {
        // Simple XOR example
        int[] layerSizes = { 2, 4, 1 }; // Input: 2, Hidden: 4, Output: 1
        NeuralNetwork nn = new NeuralNetwork(layerSizes, 0.5);

        double[][] trainingInputs = new double[][]
        {
            new double[] { 0, 0 },
            new double[] { 0, 1 },
            new double[] { 1, 0 },
            new double[] { 1, 1 }
        };

        double[][] trainingOutputs = new double[][]
        {
            new double[] { 0 },
            new double[] { 1 },
            new double[] { 1 },
            new double[] { 0 }
        };

        // Train
        for (int epoch = 0; epoch < 10000; epoch++)
        {
            for (int i = 0; i < trainingInputs.Length; i++)
            {
                nn.Backpropagate(trainingInputs[i], trainingOutputs[i]);
            }
        }

        // Test
        Console.WriteLine("XOR Results:");
        for (int i = 0; i < trainingInputs.Length; i++)
        {
            double[] output = nn.Forward(trainingInputs[i]);
            Console.WriteLine($"Input: {trainingInputs[i][0]}, {trainingInputs[i][1]} => Output: {output[0]:F4}");
        }
    }
}
