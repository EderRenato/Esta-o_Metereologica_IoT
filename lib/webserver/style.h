#define STYLE_PAGE \
"\nbody {" \
"\n    font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;" \
"\n    margin: 0;" \
"\n    padding: 0;" \
"\n    background-color: #f0f2f5;" \
"\n    color: #333;" \
"\n}" \
"\n.container {" \
"\n    max-width: 1200px;" \
"\n    margin: 0 auto;" \
"\n    padding: 20px;" \
"\n}" \
"\nheader {" \
"\n    background-color: #2c3e50;" \
"\n    color: white;" \
"\n    padding: 20px 0;" \
"\n    text-align: center;" \
"\n    border-radius: 8px 8px 0 0;" \
"\n    margin-bottom: 20px;" \
"\n}" \
"\nh1, h2, h3 {" \
"\n    margin: 0;" \
"\n}" \
"\nh1 {" \
"\n    font-size: 2.2em;" \
"\n}" \
"\nh2 {" \
"\n    color: #2c3e50;" \
"\n    font-size: 1.8em;" \
"\n    margin-bottom: 15px;" \
"\n}" \
"\nh3 {" \
"\n    color: #34495e;" \
"\n    font-size: 1.3em;" \
"\n    margin-bottom: 15px;" \
"\n}" \
"\n.config-panel {" \
"\n    background-color: white;" \
"\n    border-radius: 8px;" \
"\n    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);" \
"\n    padding: 20px;" \
"\n    margin-bottom: 20px;" \
"\n}" \
"\n.config-forms {" \
"\n    display: grid;" \
"\n    grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));" \
"\n    gap: 20px;" \
"\n}" \
"\n.config-form {" \
"\n    background-color: #f8f9fa;" \
"\n    padding: 15px;" \
"\n    border-radius: 6px;" \
"\n}" \
"\n.form-group {" \
"\n    margin-bottom: 15px;" \
"\n}" \
"\n.form-group label {" \
"\n    display: block;" \
"\n    margin-bottom: 5px;" \
"\n    font-weight: 600;" \
"\n    color: #7f8c8d;" \
"\n}" \
"\n.form-group input {" \
"\n    width: 100%;" \
"\n    padding: 8px;" \
"\n    border: 1px solid #ddd;" \
"\n    border-radius: 4px;" \
"\n    font-size: 1em;" \
"\n}" \
"\nbutton {" \
"\n    background-color: #3498db;" \
"\n    color: white;" \
"\n    border: none;" \
"\n    padding: 10px 15px;" \
"\n    border-radius: 4px;" \
"\n    cursor: pointer;" \
"\n    font-size: 1em;" \
"\n    transition: background-color 0.3s;" \
"\n}" \
"\nbutton:hover {" \
"\n    background-color: #2980b9;" \
"\n}" \
"\n.dashboard {" \
"\n    display: grid;" \
"\n    grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));" \
"\n    gap: 20px;" \
"\n    margin-bottom: 20px;" \
"\n}" \
"\n.sensor-card {" \
"\n    background-color: white;" \
"\n    border-radius: 8px;" \
"\n    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);" \
"\n    padding: 20px;" \
"\n    transition: transform 0.3s ease;" \
"\n}" \
"\n.sensor-card:hover {" \
"\n    transform: translateY(-5px);" \
"\n}" \
"\n.sensor-header {" \
"\n    display: flex;" \
"\n    align-items: center;" \
"\n    margin-bottom: 15px;" \
"\n    border-bottom: 1px solid #eee;" \
"\n    padding-bottom: 10px;" \
"\n}" \
"\n.sensor-icon {" \
"\n    font-size: 2em;" \
"\n    margin-right: 15px;" \
"\n    color: #3498db;" \
"\n}" \
"\n.sensor-title {" \
"\n    font-size: 1.5em;" \
"\n    margin: 0;" \
"\n}" \
"\n.data-item {" \
"\n    margin-bottom: 10px;" \
"\n    display: flex;" \
"\n    justify-content: space-between;" \
"\n    align-items: center;" \
"\n}" \
"\n.data-label {" \
"\n    font-weight: 600;" \
"\n    color: #7f8c8d;" \
"\n}" \
"\n.data-value {" \
"\n    font-weight: 700;" \
"\n    font-size: 1.1em;" \
"\n}" \
"\n.data-alert {" \
"\n    font-size: 0.9em;" \
"\n    padding: 3px 6px;" \
"\n    border-radius: 3px;" \
"\n    margin-left: 10px;" \
"\n}" \
"\n.temp {" \
"\n    color: #e74c3c;" \
"\n}" \
"\n.humidity {" \
"\n    color: #3498db;" \
"\n}" \
"\n.pressure {" \
"\n    color: #9b59b6;" \
"\n}" \
"\n.altitude {" \
"\n    color: #2ecc71;" \
"\n}" \
"\n.average {" \
"\n    color: #f39c12;" \
"\n}" \
"\n.alert-high {" \
"\n    background-color: #e74c3c;" \
"\n    color: white;" \
"\n}" \
"\n.alert-low {" \
"\n    background-color: #f39c12;" \
"\n    color: white;" \
"\n}" \
"\n.last-update {" \
"\n    text-align: right;" \
"\n    font-style: italic;" \
"\n    color: #7f8c8d;" \
"\n    font-size: 0.9em;" \
"\n}" \
"\n.charts-container {" \
"\n    display: grid;" \
"\n    grid-template-columns: 1fr;" \
"\n    gap: 20px;" \
"\n    margin-top: 20px;" \
"\n}" \
"\n.chart-container {" \
"\n    background-color: white;" \
"\n    border-radius: 8px;" \
"\n    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);" \
"\n    padding: 20px;" \
"\n}" \
"\n.chart-title {" \
"\n    margin-top: 0;" \
"\n    margin-bottom: 15px;" \
"\n}" \
"\n@media (max-width: 768px) {" \
"\n    .dashboard, .config-forms {" \
"\n        grid-template-columns: 1fr;" \
"\n    }" \
"\n}" \
"\n.loading {" \
"\n    text-align: center;" \
"\n    padding: 20px;" \
"\n    color: #7f8c8d;" \
"\n}"