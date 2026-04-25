import pandas as pd
import streamlit as st
import plotly.graph_objects as go
from datetime import timedelta

st.set_page_config(page_title="Sensor Dashboard App", layout="wide")

SHEET_CSV_URL = "https://docs.google.com/spreadsheets/d/e/2PACX-1vQiftlimfhaJrKALU1zfJDUsXro3diDlJa7CrLLu8ZTcXVLYMynsfpq7GAAbTcXXd_DTmA9QOEUEnN1/pub?output=csv"

if st.sidebar.button("Refresh data"):
    st.cache_data.clear()


@st.cache_data(ttl=5)
def load_data():
    df = pd.read_csv(SHEET_CSV_URL)
    df = df.dropna(how="all")
    df.columns = [str(c).strip().lower() for c in df.columns]

    if "timestamp" not in df.columns:
        df = pd.read_csv(SHEET_CSV_URL, skiprows=1)
        df = df.dropna(how="all")
        df.columns = [str(c).strip().lower() for c in df.columns]

    required_cols = ["timestamp", "temperature", "humidity", "sound"]
    missing = [col for col in required_cols if col not in df.columns]
    if missing:
        st.error(f"Missing required columns: {missing}")
        st.stop()

    df["timestamp"] = pd.to_datetime(df["timestamp"], errors="coerce")
    df["temperature"] = pd.to_numeric(df["temperature"], errors="coerce")
    df["humidity"] = pd.to_numeric(df["humidity"], errors="coerce")
    df["sound"] = pd.to_numeric(df["sound"], errors="coerce")

    df = df.dropna(subset=["timestamp", "temperature", "humidity", "sound"]).copy()
    df = df.sort_values("timestamp").reset_index(drop=True)
    df["temperature_f"] = (df["temperature"] * 9 / 5) + 32

    return df


def build_interactive_chart(data: pd.DataFrame, selected_series: list[str]) -> go.Figure:
    fig = go.Figure()

    series_map = {
        "Temperature (F)": ("temperature_f", "y"),
        "Humidity (%)": ("humidity", "y2"),
        "Sound Level": ("sound", "y3"),
    }

    for label in selected_series:
        column, axis = series_map[label]
        fig.add_trace(
            go.Scatter(
                x=data["timestamp"],
                y=data[column],
                mode="lines+markers",
                name=label,
                yaxis=axis,
            )
        )

    fig.update_layout(
        height=550,
        hovermode="x unified",
        margin=dict(l=40, r=40, t=40, b=40),
        xaxis=dict(
            title="Time",
            rangeslider=dict(visible=True),
            type="date",
        ),
        yaxis=dict(
            title="Temperature (F)",
            side="left",
            showgrid=True,
        ),
        yaxis2=dict(
            title="Humidity (%)",
            overlaying="y",
            side="right",
            showgrid=False,
        ),
        yaxis3=dict(
            title="Sound Level",
            overlaying="y",
            side="right",
            anchor="free",
            position=0.92,
            showgrid=False,
        ),
        legend=dict(
            orientation="h",
            yanchor="bottom",
            y=1.02,
            xanchor="left",
            x=0,
        ),
    )

    return fig


st.title("Sensor Dashboard App")
st.caption("Temperature, humidity, and sound from Google Sheets")

df = load_data()

if df.empty:
    st.warning("No valid data found.")
    st.stop()

hours_to_show = st.sidebar.slider("Hours of history", 1, 168, 24)
rows_to_show = st.sidebar.slider("Recent rows", 5, 100, 20)

cutoff = df["timestamp"].max() - timedelta(hours=hours_to_show)
filtered = df[df["timestamp"] >= cutoff].copy()
if filtered.empty:
    filtered = df.copy()

latest = filtered.iloc[-1]

temp_delta = None
humidity_delta = None
sound_delta = None
if len(filtered) >= 2:
    temp_delta = latest["temperature_f"] - filtered.iloc[-2]["temperature_f"]
    humidity_delta = latest["humidity"] - filtered.iloc[-2]["humidity"]
    sound_delta = latest["sound"] - filtered.iloc[-2]["sound"]

c1, c2, c3, c4, c5 = st.columns(5)
c1.metric(
    "Latest temperature",
    f"{latest['temperature_f']:.1f} F",
    None if temp_delta is None else f"{temp_delta:+.1f} F",
)
c2.metric(
    "Latest humidity",
    f"{latest['humidity']:.1f} %",
    None if humidity_delta is None else f"{humidity_delta:+.1f} %",
)
c3.metric(
    "Latest sound",
    f"{latest['sound']:.0f}",
    None if sound_delta is None else f"{sound_delta:+.0f}",
)
c4.metric("Last updated", latest["timestamp"].strftime("%Y-%m-%d %H:%M:%S"))
c5.metric("Rows loaded", len(df))

st.subheader("Interactive comparison chart")

show_temp = st.checkbox("Temperature (F)", value=True)
show_humidity = st.checkbox("Humidity (%)", value=True)
show_sound = st.checkbox("Sound Level", value=True)

selected_series = []
if show_temp:
    selected_series.append("Temperature (F)")
if show_humidity:
    selected_series.append("Humidity (%)")
if show_sound:
    selected_series.append("Sound Level")

if not selected_series:
    st.info("Select at least one series.")
else:
    fig = build_interactive_chart(filtered, selected_series)
    st.plotly_chart(fig, use_container_width=True)

left, center, right = st.columns(3)

with left:
    st.subheader("Temperature (F)")
    st.line_chart(
        filtered.set_index("timestamp")[["temperature_f"]].rename(
            columns={"temperature_f": "Temperature (F)"}
        )
    )

with center:
    st.subheader("Humidity (%)")
    st.line_chart(
        filtered.set_index("timestamp")[["humidity"]].rename(
            columns={"humidity": "Humidity (%)"}
        )
    )

with right:
    st.subheader("Sound Level")
    st.line_chart(
        filtered.set_index("timestamp")[["sound"]].rename(
            columns={"sound": "Sound Level"}
        )
    )

st.subheader("Recent readings")
recent_df = filtered.tail(rows_to_show).iloc[::-1].copy()
recent_df["timestamp"] = recent_df["timestamp"].dt.strftime("%Y-%m-%d %H:%M:%S")
recent_df = recent_df[["timestamp", "temperature_f", "humidity", "sound"]]
recent_df = recent_df.rename(
    columns={
        "timestamp": "Timestamp",
        "temperature_f": "Temperature (F)",
        "humidity": "Humidity (%)",
        "sound": "Sound Level",
    }
)
st.dataframe(recent_df, use_container_width=True)

st.subheader("Summary statistics")
summary = pd.DataFrame(
    {
        "Metric": ["Temperature (F)", "Humidity (%)", "Sound Level"],
        "Min": [
            filtered["temperature_f"].min(),
            filtered["humidity"].min(),
            filtered["sound"].min(),
        ],
        "Max": [
            filtered["temperature_f"].max(),
            filtered["humidity"].max(),
            filtered["sound"].max(),
        ],
        "Mean": [
            filtered["temperature_f"].mean(),
            filtered["humidity"].mean(),
            filtered["sound"].mean(),
        ],
    }
)
st.dataframe(summary, use_container_width=True)