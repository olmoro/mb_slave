/*=====================================================================================
 * Description:
 *    ESP32
 *====================================================================================*/
#ifndef _STAFF_H_
#define _STAFF_H_



/* Параметры функции:
.
*/

/**
 * @brief Побайтное копирование со стаффингом.
 *
 * @param src: исходный буфер.
 * @param dest: целевой буфер.
 * @param src_len: длина исходного буфера.
 * @param dest_max_len: максимально допустимая длина целевого буфера.
 * @param dest_actual_len: указатель для возврата фактической длины данных после обработки
 *
 * @return
 *        true: данные успешно обработаны и записаны.
 *        false: недостаточно места в целевом буфере.
 */
bool staffProcess(const uint8_t *src, uint8_t *dest, size_t src_len, size_t dest_max_len, size_t *dest_actual_len);

#endif  // _STAFF_H_
