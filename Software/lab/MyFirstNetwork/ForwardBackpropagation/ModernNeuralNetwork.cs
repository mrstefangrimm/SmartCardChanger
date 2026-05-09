

public class ModernNeuralNetwork
{
    private double[][] weights;
    private double[][] biases;
    private double learningRate;
    private Random random;

    public ModernNeuralNetwork(int[] layerSizes, double learningRate = 0.01)
    {
        this.learningRate = learningRate;
        this.random = new Random();
        InitializeWeights(layerSizes);
    }

    private void InitializeWeights(int[] layerSizes)
    {
        weights = new double[layerSizes.Length - 1][];
        biases = new double[layerSizes.Length - 1][];

        for (int i = 0; i < layerSizes.Length - 1; i++)
        {
            int inputSize = layerSizes[i];
            int outputSize = layerSizes[i + 1];

            weights[i] = new double[inputSize * outputSize];
            biases[i] = new double[outputSize];

            //He initialization(good for ReLU)
            double stdDev = Math.Sqrt(2.0 / inputSize);
            for (int j = 0; j < weights[i].Length; j++)
            {
                weights[i][j] = (random.NextDouble() - 0.5) * 2 * stdDev;
            }

            //Biases typically initialized to zero

            for (int j = 0; j < biases[i].Length; j++)
            {
                biases[i][j] = 0;
            }
        }
    }

    //ReLU activation: max(0, x)
    private double ReLU(double x)
    {
        return Math.Max(0, x);
    }

    private double ReLUDerivative(double x)
    {
        return x > 0 ? 1 : 0;
    }

    //Softmax for output layer(multi-class classification)
    private double[] Softmax(double[] logits)
    {
        double max = logits.Max();
        double[] exp = new double[logits.Length];
        double sum = 0;

        for (int i = 0; i < logits.Length; i++)
        {
            exp[i] = Math.Exp(logits[i] - max); // Subtract max for numerical stability
            sum += exp[i];
        }

        for (int i = 0; i < exp.Length; i++)
        {
            exp[i] /= sum;
        }

        return exp;
    }

    public double[] Forward(double[] input)
    {
        double[] current = input;

        //Forward through hidden layers with ReLU

        for (int layer = 0; layer < weights.Length - 1; layer++)
        {
            current = ForwardLayer(current, layer, true);
        }

        //Forward through output layer with Softmax

        current = ForwardLayer(current, weights.Length - 1, false);

        return current;
    }

    private double[] ForwardLayer(double[] input, int layerIndex, bool useReLU)
    {
        int outputSize = biases[layerIndex].Length;
        double[] output = new double[outputSize];

        for (int j = 0; j < outputSize; j++)
        {
            double sum = biases[layerIndex][j];
            for (int i = 0; i < input.Length; i++)
            {
                sum += input[i] * weights[layerIndex][i * outputSize + j];
            }

            if (useReLU)
            {
                output[j] = ReLU(sum);
            }
            else
            {
                output[j] = sum; // Pre-activation for softmax
            }
        }

        //Apply softmax only on final layer

        if (!useReLU && layerIndex == weights.Length - 1)
        {
            output = Softmax(output);
        }

        return output;
    }

    public void Train(double[] input, double[] expectedOutput)
    {
        //Forward pass -store all activations

        double[][] activations = new double[weights.Length + 1][];
        double[][] preActivations = new double[weights.Length][]; // Pre-activation values for ReLU
        activations[0] = input;

        double[] current = input;
        for (int layer = 0; layer < weights.Length; layer++)
        {
            int outputSize = biases[layer].Length;
            preActivations[layer] = new double[outputSize];
            double[] output = new double[outputSize];

            for (int j = 0; j < outputSize; j++)
            {
                double sum = biases[layer][j];
                for (int i = 0; i < current.Length; i++)
                {
                    sum += current[i] * weights[layer][i * outputSize + j];
                }
                preActivations[layer][j] = sum;

                if (layer < weights.Length - 1)
                {
                    output[j] = ReLU(sum);
                }
                else
                {
                    output[j] = sum;
                }
            }

            //Apply softmax to output layer

            if (layer == weights.Length - 1)
            {
                output = Softmax(output);
            }

            activations[layer + 1] = output;
            current = output;
        }

        //Backward pass

        double[] deltas = new double[biases[weights.Length - 1].Length];
        for (int i = 0; i < deltas.Length; i++)
        {
            //Cross - entropy derivative: (output - expected)

            deltas[i] = activations[weights.Length][i] - expectedOutput[i];
        }

        //Update weights and biases layer by layer

        for (int layer = weights.Length - 1; layer >= 0; layer--)
        {
            double[] layerActivations = activations[layer];
            int outputSize = biases[layer].Length;

            //Update biases

            for (int j = 0; j < outputSize; j++)
            {
                biases[layer][j] -= learningRate * deltas[j];
            }

            //Update weights

            for (int j = 0; j < outputSize; j++)
            {
                for (int i = 0; i < layerActivations.Length; i++)
                {
                    int index = i * outputSize + j;
                    weights[layer][index] -= learningRate * deltas[j] * layerActivations[i];
                }
            }

            //Compute deltas for previous layer

            if (layer > 0)
            {
                double[] prevDeltas = new double[layerActivations.Length];
                for (int i = 0; i < prevDeltas.Length; i++)
                {
                    double sum = 0;
                    for (int j = 0; j < outputSize; j++)
                    {
                        sum += deltas[j] * weights[layer][i * outputSize + j];
                    }
                    //Apply ReLU derivative for hidden layers




                    prevDeltas[i] = sum * ReLUDerivative(preActivations[layer - 1][i]);


                }
                deltas = prevDeltas;
            }
        }
    }

    public static void AMain()
    {
        //Multi -class classification example(3 classes)

        int[] layerSizes = { 2, 16, 8, 3 }; // Input: 2, Hidden: 16, Hidden: 8, Output: 3

        ModernNeuralNetwork nn = new ModernNeuralNetwork(layerSizes, 0.1);

        //Training data: simple 3-class problem

        double[][] trainingInputs = new double[][]
        {
            new double[] { 0.1, 0.1 },
            new double[] { 0.1, 0.9 },
            new double[] { 0.9, 0.1 },
            new double[] { 0.9, 0.9 },
            new double[] { 0.5, 0.5 },
            new double[] { 0.2, 0.8 }
        };

        double[][] trainingOutputs = new double[][]
        {
            new double[] { 1, 0, 0 }, // Class 0
            new double[] { 0, 1, 0 }, // Class 1
            new double[] { 0, 1, 0 }, // Class 1
            new double[] { 0, 0, 1 }, // Class 2
            new double[] { 0, 1, 0 }, // Class 1
            new double[] { 0, 1, 0 }  // Class 1
		};

        //Train
        for (int epoch = 0; epoch < 5000; epoch++)
        {
            for (int i = 0; i < trainingInputs.Length; i++)
            {
                nn.Train(trainingInputs[i], trainingOutputs[i]);
            }
        }

        //Test
        Console.WriteLine("Classification Results (Probabilities):");
        for (int i = 0; i < trainingInputs.Length; i++)
        {
            double[] output = nn.Forward(trainingInputs[i]);
            int predictedClass = Array.IndexOf(output, output.Max());
            Console.WriteLine($"Input: ({trainingInputs[i][0]:F1}, {trainingInputs[i][1]:F1}) " +
                $"=> Class {predictedClass}, Probabilities: [{string.Join(", ", output.Select(x => x.ToString("F4")))}]");
        }
    }
}


