message(${UI_NAME} ${UI_FILE} ${HEADER_FILE})
file(READ ${UI_FILE} UI_CONTENT)
file(WRITE ${HEADER_FILE} constexpr\ const\ char\ *k${UI_NAME}UIString\ =\ R\"\()
file(APPEND ${HEADER_FILE} ${UI_CONTENT})
file(APPEND ${HEADER_FILE} \)\"\;)