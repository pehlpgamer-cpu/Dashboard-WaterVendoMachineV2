import jsPDF from 'jspdf'
import { WaterLog } from './firestoreService'

export class ExportService {
  // Export to CSV
  exportToCSV(logs: WaterLog[], filename = 'water-sales.csv'): void {
    const headers = ['Timestamp', 'Water Type', 'Amount', 'Date']
    const rows = logs.map(log => [
      new Date(log.timestamp).toLocaleString(),
      log.isCold ? 'Cold' : 'Regular',
      `$${log.amount}`,
      new Date(log.timestamp).toISOString().split('T')[0]
    ])

    const csvContent = [
      headers.join(','),
      ...rows.map(row => row.map(cell => `"${cell}"`).join(','))
    ].join('\n')

    const blob = new Blob([csvContent], { type: 'text/csv' })
    const url = window.URL.createObjectURL(blob)
    const link = document.createElement('a')
    link.href = url
    link.download = filename
    link.click()
    window.URL.revokeObjectURL(url)
  }

  // Export to PDF
  exportToPDF(logs: WaterLog[], filename = 'water-sales.pdf'): void {
    const doc = new jsPDF()
    doc.setFontSize(16)
    doc.text('Water Sales Report', 10, 10)

    doc.setFontSize(10)
    const tableData = logs.map(log => [
      new Date(log.timestamp).toLocaleString(),
      log.isCold ? 'Cold' : 'Regular',
      `$${log.amount}`
    ])

    // Create table using autoTable plugin (if available) or manual
    let y = 30
    const headers = ['Timestamp', 'Type', 'Amount']

    doc.setFont(undefined, 'bold')
    headers.forEach((header, i) => {
      doc.text(header, 10 + i * 60, y)
    })

    doc.setFont(undefined, 'normal')
    y += 10

    tableData.forEach((row, idx) => {
      if (y > 270) {
        doc.addPage()
        y = 10
      }
      row.forEach((cell, i) => {
        doc.text(cell, 10 + i * 60, y)
      })
      y += 8
    })

    doc.save(filename)
  }

  // Export to Excel (XLSX)
  async exportToExcel(logs: WaterLog[], filename = 'water-sales.xlsx'): Promise<void> {
    const XLSX = await import('xlsx')

    const data = logs.map(log => ({
      Timestamp: new Date(log.timestamp).toLocaleString(),
      'Water Type': log.isCold ? 'Cold' : 'Regular',
      Amount: log.amount,
      Date: new Date(log.timestamp).toISOString().split('T')[0]
    }))

    const ws = XLSX.utils.json_to_sheet(data)
    const wb = XLSX.utils.book_new()
    XLSX.utils.book_append_sheet(wb, ws, 'Sales')

    // Auto-size columns
    const colWidths = [{ wch: 25 }, { wch: 15 }, { wch: 10 }, { wch: 15 }]
    ws['!cols'] = colWidths

    XLSX.writeFile(wb, filename)
  }
}

export default new ExportService()
