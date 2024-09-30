import pandas as pd
import matplotlib.pyplot as plt

# Read the CSV file
data = pd.read_csv('convergence_plot.csv')

# Display the first few rows of the dataframe
print(data.head())

# Create a scatter plot
plt.plot(data['x'], data['y'])

# Add labels and title
plt.xlabel('X')
plt.ylabel('Y')
plt.title('test plot')

# Show the plot
plt.show()
