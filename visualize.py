from math import sin, pi
from pathlib import Path
from random import random
from time import perf_counter

import pandas as pd
import matplotlib.dates as mdates
import matplotlib.pyplot as plt

DATAFILE_PATH = "datalog.csv"
TESTFILE_PATH = "testlog.csv"

def generate_test_data():
    # Make the date index of the test data
    test_datetime = pd.date_range(
        "2022-04-13 13:05:53",
        "2022-04-17 16:32:45",
        freq = 'T',  # minute
    )
    
    st_hr = test_datetime[0].hour
    cl_hr = 6

    # Model the voltage as a clipped sine wave with period equal to one day
    # (1440 minutes) and trough centered at `cl_hr` hours into the day
    noise_offset = random()
    test_data = pd.Series(
        [
            min(
                  12.5 
                + 0.5*sin(2*pi*i/2000 - noise_offset)
                + 2*sin(2*pi*(i + (st_hr - cl_hr)*60)/1440 - pi/2)
            , 14)
            for i in range(len(test_datetime))
        ],
        test_datetime,
    )

    test_data.to_csv(TESTFILE_PATH, header = None)


def load_data(csv_path: str) -> pd.DataFrame:
    data = pd.read_csv(
        csv_path,
        header = 0,
        names = ["Datetime", "Voltage"],
        index_col = "Datetime",
        parse_dates = True,
    )

    # Convert the index to DatetimeIndex to allow resampling
    data.index = pd.DatetimeIndex(data.index)

    return data


def gen_daily_plots(data: pd.DataFrame, dir_path: str):
    # Create directory path if it doesn't already exist
    Path(dir_path).mkdir(parents = True, exist_ok = True)

    # Create list of daily data and iterate through it
    days = list(data.resample("D"))
    for datetime, df in days:
        # Get string representation of current date
        date = str(datetime.date())

        # Start making current plot
        plt.figure("current")
        plt.plot(df["Voltage"])
        fmt = mdates.DateFormatter('%H:%M')
        plt.gcf().axes[0].xaxis.set_major_formatter(fmt)
        plt.ylabel("Voltage (V)")
        plt.xlabel("Time")
        plt.ylim([10, 15])
        plt.title(date)
        plt.grid()

        # Save plot as image in specified directory and reset figure
        plt.savefig(Path(dir_path)/f"{date}.png")
        plt.clf()

        # Discard dates from index
        df.index = pd.to_datetime(
            (df.index - df.index.normalize())
            .to_numpy().astype(pd.Timestamp)
        )

        # Add this data to cumulative figure
        plt.figure("cumulative")
        plt.plot(df["Voltage"]) 

    # Generate the cumulative plot
    plt.figure("cumulative")
    fmt = mdates.DateFormatter('%H:%M')
    plt.gcf().axes[0].xaxis.set_major_formatter(fmt)
    plt.title(f"{days[0][0].date()} to {days[-1][0].date()}")
    plt.ylabel("Voltage (V)")
    plt.xlabel("Time")
    plt.grid()

    plt.savefig(Path(dir_path)/f"cumulative.png")


if __name__ == "__main__":
    start = perf_counter()
    generate_test_data()
    gen_daily_plots(load_data(TESTFILE_PATH), "data/test/daily")
    print(f"Test files generated in {(perf_counter() - start)*1e3:.3f} ms")
