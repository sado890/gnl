/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_get_next_line.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: muarici <muarici@student.42kocaeli.com.tr> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/15 19:12:31 by muarici           #+#    #+#             */
/*   Updated: 2026/03/21 by muarici                  ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 42
#endif

static char	storage[BUFFER_SIZE + 1];

static int	ft_strlen(char *s)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
		i++;
	return (i);
}

static char	*ft_strjoin(char *s1, char *s2)
{
	char	*result;
	int		len1;
	int		len2;
	int		i;

	if (!s2)
		return (s1);
	len1 = ft_strlen(s1);
	len2 = ft_strlen(s2);
	result = malloc(sizeof(char) * (len1 + len2 + 1));
	if (!result)
	{
		free(s1);
		return (NULL);
	}
	i = 0;
	while (i < len1)
	{
		result[i] = s1[i];
		i++;
	}
	i = 0;
	while (i < len2)
	{
		result[len1 + i] = s2[i];
		i++;
	}
	result[len1 + len2] = '\0';
	free(s1);
	return (result);
}

static int	check_storage(void)
{
	int	i;

	i = 0;
	while (storage[i])
	{
		if (storage[i] == '\n')
			return (i);
		i++;
	}
	return (-1);
}

static void	clear_storage(int x)
{
	int	i;

	i = 0;
	while (storage[x] != '\0')
	{
		storage[i] = storage[x];
		i++;
		x++;
	}
	storage[i] = '\0';
}

static char	*ft_get_storage(int i)
{
	char	*returnval;
	int		j;

	j = 0;
	if (storage[0] == '\0')
		return (NULL);
	if (i == -1)
	{
		j = ft_strlen(storage);
		returnval = malloc(sizeof(char) * (j + 1));
		if (!returnval)
			return (NULL);
		j = 0;
		while (storage[j])
		{
			returnval[j] = storage[j];
			j++;
		}
		returnval[j] = '\0';
		storage[0] = '\0';
	}
	else
	{
		returnval = malloc(sizeof(char) * (i + 2));
		if (!returnval)
			return (NULL);
		while (j <= i)
		{
			returnval[j] = storage[j];
			j++;
		}
		returnval[j] = '\0';
		clear_storage(j);
	}
	return (returnval);
}

char	*get_next_line(int fd)
{
	int		bytes_read;
	int		newline_pos;
	char	*chunk;
	char	*result;

	if (fd < 0 || BUFFER_SIZE <= 0)
		return (NULL);
	result = NULL;
	while (1)
	{
		newline_pos = check_storage();
		if (newline_pos >= 0)
		{
			chunk = ft_get_storage(newline_pos);
			result = ft_strjoin(result, chunk);
			free(chunk);
			return (result);
		}
		chunk = ft_get_storage(-1);
		if (chunk)
		{
			result = ft_strjoin(result, chunk);
			free(chunk);
		}
		bytes_read = read(fd, storage, BUFFER_SIZE);
		storage[bytes_read > 0 ? bytes_read : 0] = '\0';
		if (bytes_read <= 0)
			break ;
	}
	return (result);
}

/* ************************************************************************** */
/*  GNL ATTACK TEST SUITE — muarici                                           */
/*  Derle: gcc -Wall -Wextra -Werror -D BUFFER_SIZE=42 gnl_attack_test.c     */
/*  Valgrind: valgrind --leak-check=full --track-origins=yes ./a.out          */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

/* ---- renk kodları ---- */
#define RED   "\033[1;31m"
#define GRN   "\033[1;32m"
#define YEL   "\033[1;33m"
#define CYN   "\033[1;36m"
#define RST   "\033[0m"

/* ---- GNL prototipi ---- */
char *get_next_line(int fd);

/* ------------------------------------------------------------------ */
/*  Yardımcı: geçici dosya yazar, fd döndürür                          */
/* ------------------------------------------------------------------ */
static int	make_tmpfile(const char *name, const char *content, int len)
{
	int fd;

	fd = open(name, O_RDWR | O_CREAT | O_TRUNC, 0600);
	if (fd < 0) { perror("open"); return (-1); }
	if (len < 0) len = (int)strlen(content);
	write(fd, content, len);
	lseek(fd, 0, SEEK_SET);
	return (fd);
}

/* ------------------------------------------------------------------ */
/*  Yardımcı: tek satır okur, beklenenle karşılaştırır                 */
/* ------------------------------------------------------------------ */
static void	assert_line(const char *test, int call_n, char *got, const char *expected)
{
	if (expected == NULL && got == NULL)
	{
		printf(GRN "[PASS]" RST " %s #%d → NULL\n", test, call_n);
		return;
	}
	if (expected == NULL && got != NULL)
	{
		printf(RED "[FAIL]" RST " %s #%d → beklenen NULL, alınan [%s]\n", test, call_n, got);
		free(got);
		return;
	}
	if (expected != NULL && got == NULL)
	{
		printf(RED "[FAIL]" RST " %s #%d → beklenen [%s], alınan NULL\n", test, call_n, expected);
		return;
	}
	if (strcmp(got, expected) == 0)
		printf(GRN "[PASS]" RST " %s #%d → [%s]\n", test, call_n, got);
	else
		printf(RED "[FAIL]" RST " %s #%d → beklenen [%s], alınan [%s]\n", test, call_n, expected, got);
	free(got);
}

/* ================================================================== */
/*  ATTACK 1: Boş dosya                                                */
/*  Beklenti: ilk çağrıda NULL                                         */
/*  Neden patlar: storage[0]=='\0', ft_get_storage NULL döner,         */
/*  result NULL kalır ama döngü read=0 ile kırılır. OK görünür ama     */
/*  valgrind'de leak kontrolü yapın.                                   */
/* ================================================================== */
static void	attack_empty_file(void)
{
	int   fd = make_tmpfile("/tmp/gnl_t1.txt", "", 0);
	char *l  = get_next_line(fd);
	assert_line("ATTACK1_empty_file", 1, l, NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 2: Sadece \n karakterleri                                   */
/*  Beklenti: "\n" "\n" "\n" NULL                                      */
/*  Neden patlar: clear_storage sonrası storage tutarlı mı?            */
/* ================================================================== */
static void	attack_only_newlines(void)
{
	int fd = make_tmpfile("/tmp/gnl_t2.txt", "\n\n\n", -1);
	assert_line("ATTACK2_only_newlines", 1, get_next_line(fd), "\n");
	assert_line("ATTACK2_only_newlines", 2, get_next_line(fd), "\n");
	assert_line("ATTACK2_only_newlines", 3, get_next_line(fd), "\n");
	assert_line("ATTACK2_only_newlines", 4, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 3: Son satırda \n yok                                       */
/*  Beklenti: "birinci\n" "ikinci" NULL                                */
/*  Neden patlar: EOF path'inde result doğru birikiyor mu?             */
/* ================================================================== */
static void	attack_no_trailing_newline(void)
{
	int fd = make_tmpfile("/tmp/gnl_t3.txt", "birinci\nikinci", -1);
	assert_line("ATTACK3_no_trailing_nl", 1, get_next_line(fd), "birinci\n");
	assert_line("ATTACK3_no_trailing_nl", 2, get_next_line(fd), "ikinci");
	assert_line("ATTACK3_no_trailing_nl", 3, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 4: BUFFER_SIZE sınırını tam aşan satır                      */
/*  BUFFER_SIZE=42; 43 karakter + \n → iki read gerekir               */
/*  Beklenti: 43 'a' + '\n' tek string olarak gelmeli                  */
/*  Neden patlar: ft_strjoin birleştirme sırası veya storage           */
/*  overwrite sorunu varsa parçalı gelir.                              */
/* ================================================================== */
static void	attack_exactly_buffer_plus_one(void)
{
	/* 43 'a' + '\n' = 44 byte */
	char content[45];
	memset(content, 'a', 43);
	content[43] = '\n';
	content[44] = '\0';

	char expected[45];
	memset(expected, 'a', 43);
	expected[43] = '\n';
	expected[44] = '\0';

	int fd = make_tmpfile("/tmp/gnl_t4.txt", content, 44);
	assert_line("ATTACK4_buf_plus_one", 1, get_next_line(fd), expected);
	assert_line("ATTACK4_buf_plus_one", 2, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 5: Çok uzun satır (1000 karakter, \n yok)                   */
/*  Beklenti: 1000 'x' tek seferde NULL olmadan gelmeli               */
/*  Neden patlar: her read döngüsünde ft_strjoin çağrılır;            */
/*  eğer storage'dan chunk alırken bir byte kaybolursa eksik gelir.   */
/* ================================================================== */
static void	attack_long_line_no_newline(void)
{
	char content[1001];
	char expected[1001];
	memset(content,  'x', 1000); content[1000]  = '\0';
	memset(expected, 'x', 1000); expected[1000] = '\0';

	int fd = make_tmpfile("/tmp/gnl_t5.txt", content, 1000);
	char *got = get_next_line(fd);
	if (got && strcmp(got, expected) == 0)
		printf(GRN "[PASS]" RST " ATTACK5_long_no_nl → 1000 karakter doğru\n");
	else if (got)
		printf(RED "[FAIL]" RST " ATTACK5_long_no_nl → uzunluk %d (beklenen 1000)\n",
			(int)strlen(got));
	else
		printf(RED "[FAIL]" RST " ATTACK5_long_no_nl → NULL döndü\n");
	free(got);
	assert_line("ATTACK5_long_no_nl", 2, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 6: \n tam BUFFER_SIZE'ın sonunda                            */
/*  BUFFER_SIZE=42; içerik: 41 'b' + '\n' + 41 'c' + '\n'            */
/*  İlk read tam 42 byte okur: 41b + \n → storage'ın son byte'ı \n   */
/*  Beklenti: "bbb...b\n" (41b) sonra "ccc...c\n" (41c)              */
/*  Neden patlar: clear_storage(42) → storage[42] okunur; bu          */
/*  storage[BUFFER_SIZE+1] dizisinin tam sınırı → off-by-one.         */
/* ================================================================== */
static void	attack_newline_at_buffer_end(void)
{
	char content[87];
	memset(content,      'b', 41); content[41] = '\n';
	memset(content + 42, 'c', 41); content[83] = '\n';
	content[84] = '\0';

	char exp1[43]; memset(exp1, 'b', 41); exp1[41] = '\n'; exp1[42] = '\0';
	char exp2[43]; memset(exp2, 'c', 41); exp2[41] = '\n'; exp2[42] = '\0';

	int fd = make_tmpfile("/tmp/gnl_t6.txt", content, 84);
	assert_line("ATTACK6_nl_at_buf_end", 1, get_next_line(fd), exp1);
	assert_line("ATTACK6_nl_at_buf_end", 2, get_next_line(fd), exp2);
	assert_line("ATTACK6_nl_at_buf_end", 3, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 7: Birden fazla \n aynı buffer içinde                       */
/*  İçerik: "a\nb\nc\n" — hepsi tek read'e sığar (6 byte < 42)       */
/*  Beklenti: "a\n" "b\n" "c\n" NULL                                  */
/*  Neden patlar: clear_storage art arda doğru kaydırıyor mu?         */
/* ================================================================== */
static void	attack_multiple_nl_in_one_read(void)
{
	int fd = make_tmpfile("/tmp/gnl_t7.txt", "a\nb\nc\n", -1);
	assert_line("ATTACK7_multi_nl_one_read", 1, get_next_line(fd), "a\n");
	assert_line("ATTACK7_multi_nl_one_read", 2, get_next_line(fd), "b\n");
	assert_line("ATTACK7_multi_nl_one_read", 3, get_next_line(fd), "c\n");
	assert_line("ATTACK7_multi_nl_one_read", 4, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 8: fd = -1                                                  */
/*  Beklenti: NULL                                                     */
/* ================================================================== */
static void	attack_invalid_fd(void)
{
	char *l = get_next_line(-1);
	assert_line("ATTACK8_invalid_fd", 1, l, NULL);
}

/* ================================================================== */
/*  ATTACK 9: fd kapatıldıktan sonra tekrar çağrı                      */
/*  Beklenti: NULL (read -1 dönecek)                                   */
/*  Neden patlar: storage static — önceki test'ten kirli veri          */
/*  kalabilir; bu çağrı o veriyi döndürürse bug var.                  */
/* ================================================================== */
static void	attack_closed_fd(void)
{
	int fd = make_tmpfile("/tmp/gnl_t9.txt", "satir\n", -1);
	get_next_line(fd); /* ilk satırı tüket */
	close(fd);
	char *l = get_next_line(fd); /* kapalı fd */
	/* read -1 döner ama storage boşsa NULL bekliyoruz */
	if (l == NULL)
		printf(GRN "[PASS]" RST " ATTACK9_closed_fd → NULL\n");
	else
		printf(RED "[FAIL]" RST " ATTACK9_closed_fd → NULL beklendi, [%s] geldi\n", l);
	free(l);
}

/* ================================================================== */
/*  ATTACK 10: Static storage kirliliği — iki farklı fd               */
/*  GNL tek static array kullanıyor; fd değişince storage             */
/*  karışır. Bu 42'nin bonus kısmı ama standart davranışı test eder.  */
/*  Aynı fd'yi kapat-aç yaparak storage reset'ini zorluyoruz.         */
/* ================================================================== */
static void	attack_storage_pollution(void)
{
	printf(YEL "[INFO]" RST " ATTACK10: static storage kirliliği testi\n");

	int fd1 = make_tmpfile("/tmp/gnl_t10a.txt", "dosya_bir\n", -1);
	int fd2 = make_tmpfile("/tmp/gnl_t10b.txt", "dosya_iki\n", -1);

	char *l1 = get_next_line(fd1);
	char *l2 = get_next_line(fd2);

	/* Standart GNL'de fd2'den okuma storage'ı patlatır */
	/* Sonuç implementasyona göre değişir ama en azından crash olmamalı */
	if (l1) { printf("  fd1 → [%s]\n", l1); free(l1); }
	else     printf("  fd1 → NULL\n");
	if (l2) { printf("  fd2 → [%s]\n", l2); free(l2); }
	else     printf("  fd2 → NULL\n");

	printf(YEL "  NOT:" RST " Standart GNL tek fd varsayar."
		" Yukarıdaki sonuç implementasyona göre değişir.\n");
	close(fd1);
	close(fd2);
}

/* ================================================================== */
/*  ATTACK 11: Null byte içeren içerik (\0 ortada)                    */
/*  ft_strlen '\0'da durur → içerik kesilir                           */
/*  Beklenti: implementasyon bağımlı ama crash olmamalı               */
/* ================================================================== */
static void	attack_null_byte_in_content(void)
{
	printf(YEL "[INFO]" RST " ATTACK11: null byte ortada\n");
	/* "abc\0def\n" — 8 byte, \0 sonrası görünmez */
	int fd = make_tmpfile("/tmp/gnl_t11.txt", "abc\0def\n", 8);
	char *l = get_next_line(fd);
	if (l)
	{
		printf("  alınan uzunluk: %d (tam olması gereken: 8)\n", (int)strlen(l));
		if (strlen(l) < 8)
			printf(RED "  [WARN]" RST " ft_strlen null byte'ta kesti — beklenen davranış\n");
		free(l);
	}
	else
		printf("  NULL döndü\n");
	close(fd);
}

/* ================================================================== */
/*  ATTACK 12: Tek karakter satırlar — 'a\n' x 500                    */
/*  Beklenti: 500 kez "a\n", sonra NULL                               */
/*  Neden patlar: clear_storage 500 kez çalışır; birikimli hata var   */
/*  mı? Ayrıca valgrind'de bellek sızıntısı görünür mü?              */
/* ================================================================== */
static void	attack_500_single_char_lines(void)
{
	printf(YEL "[INFO]" RST " ATTACK12: 500 satır 'a\\n'\n");

	/* 1000 byte dosya oluştur */
	char *content = malloc(1001);
	int i = 0;
	while (i < 1000) { content[i] = 'a'; content[i+1] = '\n'; i += 2; }
	content[1000] = '\0';

	int fd = make_tmpfile("/tmp/gnl_t12.txt", content, 1000);
	free(content);

	int count = 0;
	int errors = 0;
	char *l;
	while ((l = get_next_line(fd)) != NULL)
	{
		count++;
		if (strcmp(l, "a\n") != 0) errors++;
		free(l);
	}
	if (count == 500 && errors == 0)
		printf(GRN "[PASS]" RST " ATTACK12 → 500 satır doğru okundu\n");
	else
		printf(RED "[FAIL]" RST " ATTACK12 → %d satır okundu, %d hata\n", count, errors);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 13: BUFFER_SIZE=1 simülasyonu                               */
/*  Derleme zamanında değiştirilemez ama içerik 1 byte'lık             */
/*  read davranışını zorlar: her read 1 byte okusun diye pipe kullan  */
/* ================================================================== */
static void	attack_one_byte_reads(void)
{
	printf(YEL "[INFO]" RST " ATTACK13: pipe üzerinden 1'er byte okuma\n");

	int pipefd[2];
	if (pipe(pipefd) < 0) { perror("pipe"); return; }

	/* "hello\nworld" yaz */
	write(pipefd[1], "hello\nworld", 11);
	close(pipefd[1]);

	assert_line("ATTACK13_pipe", 1, get_next_line(pipefd[0]), "hello\n");
	assert_line("ATTACK13_pipe", 2, get_next_line(pipefd[0]), "world");
	assert_line("ATTACK13_pipe", 3, get_next_line(pipefd[0]), NULL);
	close(pipefd[0]);
}

/* ================================================================== */
/*  ATTACK 14: Sadece bir \n karakteri                                 */
/*  Beklenti: "\n" sonra NULL                                          */
/* ================================================================== */
static void	attack_single_newline(void)
{
	int fd = make_tmpfile("/tmp/gnl_t14.txt", "\n", -1);
	assert_line("ATTACK14_single_nl", 1, get_next_line(fd), "\n");
	assert_line("ATTACK14_single_nl", 2, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 15: storage[0]='\0' sonrası tekrar dolu storage            */
/*  Senaryo: EOF'tan sonra aynı fd'yi reopen edip tekrar okumak       */
/*  storage static olduğu için önceki state kalır mı?                 */
/* ================================================================== */
static void	attack_reopen_same_file(void)
{
	printf(YEL "[INFO]" RST " ATTACK15: aynı dosyayı kapat-tekrar aç\n");

	make_tmpfile("/tmp/gnl_t15.txt", "yeniden\n", -1);

	int fd = open("/tmp/gnl_t15.txt", O_RDONLY);
	assert_line("ATTACK15_reopen", 1, get_next_line(fd), "yeniden\n");
	assert_line("ATTACK15_reopen", 2, get_next_line(fd), NULL);
	close(fd);

	/* aynı dosyayı tekrar aç */
	fd = open("/tmp/gnl_t15.txt", O_RDONLY);
	assert_line("ATTACK15_reopen", 3, get_next_line(fd), "yeniden\n");
	assert_line("ATTACK15_reopen", 4, get_next_line(fd), NULL);
	close(fd);
}

/* ------------------------------------------------------------------ */
int	main(void)
{
	printf(CYN "\n=== GNL ATTACK TEST SUITE ===\n\n" RST);

	attack_empty_file();
	printf("\n");
	attack_only_newlines();
	printf("\n");
	attack_no_trailing_newline();
	printf("\n");
	attack_exactly_buffer_plus_one();
	printf("\n");
	attack_long_line_no_newline();
	printf("\n");
	attack_newline_at_buffer_end();
	printf("\n");
	attack_multiple_nl_in_one_read();
	printf("\n");
	attack_invalid_fd();
	printf("\n");
	attack_closed_fd();
	printf("\n");
	attack_storage_pollution();
	printf("\n");
	attack_null_byte_in_content();
	printf("\n");
	attack_500_single_char_lines();
	printf("\n");
	attack_one_byte_reads();
	printf("\n");
	attack_single_newline();
	printf("\n");
	attack_reopen_same_file();

	printf(CYN "\n=== BİTTİ — valgrind ile tekrar çalıştırın===\n\n" RST);
	printf(YEL "\n[NOT] ATTACK9 leak hakkında:\n" RST);
	printf("  Bu testte 'free(l)' if-else dışına alınmıştır.\n");
	printf("  free(NULL) C standardında güvenlidir (ISO C99 §7.20.3.2).\n");
	printf("  Eğer leak görüyorsanız, GNL kodunuzda değil test kodunda\n");
	printf("  'free' eksikliği vardır — önce test kodunu kontrol edin.\n\n");
	printf("  free(1) bu testte sadece fail durumunda çalıştığı için.\n");
	printf("  Eğer kodunuz testlerde 15/15 yaptıysa leaki görmezden gelin.\n\n");
	printf("valgrind --leak-check=full --track-origins=yes --error-exitcode=1 ./a.out\n\n");
	return (0);
}


/* ************************************************************************** */
/*                                                                            */
/*  GNL ATTACK TEST SUITE                                                     */
/*  A comprehensive stress-test for get_next_line (42 School project)         */
/*                                                                            */
/*  Compile:  gcc -Wall -Wextra -Werror -D BUFFER_SIZE=42                    */
/*            gnl_attack_test.c get_next_line.c get_next_line_utils.c        */
/*            -o gnl_test                                                     */
/*                                                                            */
/*  Valgrind: valgrind --leak-check=full --track-origins=yes                 */
/*            --error-exitcode=1 ./gnl_test                                  */
/*                                                                            */
/*  Note: Adjust BUFFER_SIZE and included source files to match your         */
/*  project structure.                                                        */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

/* ---- Color codes ---- */
#define RED   "\033[1;31m"
#define GRN   "\033[1;32m"
#define YEL   "\033[1;33m"
#define CYN   "\033[1;36m"
#define RST   "\033[0m"

/* ---- GNL prototype ---- */
char	*get_next_line(int fd);

/* ---- Global counters ---- */
static int	g_pass = 0;
static int	g_fail = 0;

/* ------------------------------------------------------------------ */
/*  Helper: write a temp file, return its fd                           */
/* ------------------------------------------------------------------ */
static int	make_tmpfile(const char *path, const char *content, int len)
{
	int	fd;

	fd = open(path, O_RDWR | O_CREAT | O_TRUNC, 0600);
	if (fd < 0)
	{
		perror("open");
		return (-1);
	}
	if (len < 0)
		len = (int)strlen(content);
	write(fd, content, len);
	lseek(fd, 0, SEEK_SET);
	return (fd);
}

/* ------------------------------------------------------------------ */
/*  Helper: compare one GNL call result against expected string        */
/* ------------------------------------------------------------------ */
static void	assert_line(const char *test, int call_n,
				char *got, const char *expected)
{
	if (expected == NULL && got == NULL)
	{
		printf(GRN "[PASS]" RST " %-30s call #%d → NULL\n", test, call_n);
		g_pass++;
		return ;
	}
	if (expected == NULL && got != NULL)
	{
		printf(RED "[FAIL]" RST " %-30s call #%d → expected NULL, got [%s]\n",
			test, call_n, got);
		free(got);
		g_fail++;
		return ;
	}
	if (expected != NULL && got == NULL)
	{
		printf(RED "[FAIL]" RST " %-30s call #%d → expected [%s], got NULL\n",
			test, call_n, expected);
		g_fail++;
		return ;
	}
	if (strcmp(got, expected) == 0)
	{
		printf(GRN "[PASS]" RST " %-30s call #%d → [%s]\n", test, call_n, got);
		g_pass++;
	}
	else
	{
		printf(RED "[FAIL]" RST " %-30s call #%d → expected [%s], got [%s]\n",
			test, call_n, expected, got);
		g_fail++;
	}
	free(got);
}

/* ================================================================== */
/*  ATTACK 1: Empty file                                               */
/*  Expected: NULL on first call                                       */
/* ================================================================== */
static void	attack_empty_file(void)
{
	int	fd;

	printf(CYN "--- ATTACK 1: Empty file ---\n" RST);
	fd = make_tmpfile("/tmp/gnl_t01.txt", "", 0);
	assert_line("empty_file", 1, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 2: Only newline characters                                  */
/*  Expected: "\n" "\n" "\n" NULL                                      */
/* ================================================================== */
static void	attack_only_newlines(void)
{
	int	fd;

	printf(CYN "--- ATTACK 2: Only newlines ---\n" RST);
	fd = make_tmpfile("/tmp/gnl_t02.txt", "\n\n\n", -1);
	assert_line("only_newlines", 1, get_next_line(fd), "\n");
	assert_line("only_newlines", 2, get_next_line(fd), "\n");
	assert_line("only_newlines", 3, get_next_line(fd), "\n");
	assert_line("only_newlines", 4, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 3: No trailing newline                                      */
/*  Expected: "first\n"  "second"  NULL                               */
/* ================================================================== */
static void	attack_no_trailing_newline(void)
{
	int	fd;

	printf(CYN "--- ATTACK 3: No trailing newline ---\n" RST);
	fd = make_tmpfile("/tmp/gnl_t03.txt", "first\nsecond", -1);
	assert_line("no_trailing_nl", 1, get_next_line(fd), "first\n");
	assert_line("no_trailing_nl", 2, get_next_line(fd), "second");
	assert_line("no_trailing_nl", 3, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 4: Line length exactly BUFFER_SIZE + 1                     */
/*  BUFFER_SIZE=42 → 43 'a' + '\n' requires two reads                 */
/*  Expected: "aaa...a\n" (43 a's) then NULL                          */
/* ================================================================== */
static void	attack_exactly_buffer_plus_one(void)
{
	char	content[45];
	char	expected[45];
	int		fd;

	printf(CYN "--- ATTACK 4: Line = BUFFER_SIZE + 1 ---\n" RST);
	memset(content, 'a', 43);
	content[43] = '\n';
	content[44] = '\0';
	memset(expected, 'a', 43);
	expected[43] = '\n';
	expected[44] = '\0';
	fd = make_tmpfile("/tmp/gnl_t04.txt", content, 44);
	assert_line("buf_plus_one", 1, get_next_line(fd), expected);
	assert_line("buf_plus_one", 2, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 5: Very long line, no newline (1000 chars)                  */
/*  Expected: all 1000 'x' chars as a single string, then NULL        */
/* ================================================================== */
static void	attack_long_line_no_newline(void)
{
	char	content[1001];
	char	expected[1001];
	char	*got;
	int		fd;

	printf(CYN "--- ATTACK 5: Long line, no newline (1000 chars) ---\n" RST);
	memset(content, 'x', 1000);
	content[1000] = '\0';
	memset(expected, 'x', 1000);
	expected[1000] = '\0';
	fd = make_tmpfile("/tmp/gnl_t05.txt", content, 1000);
	got = get_next_line(fd);
	if (got && strcmp(got, expected) == 0)
	{
		printf(GRN "[PASS]" RST " %-30s call #1 → 1000 chars correct\n",
			"long_no_nl");
		g_pass++;
	}
	else if (got)
	{
		printf(RED "[FAIL]" RST " %-30s call #1 → length %d (expected 1000)\n",
			"long_no_nl", (int)strlen(got));
		g_fail++;
	}
	else
	{
		printf(RED "[FAIL]" RST " %-30s call #1 → returned NULL\n", "long_no_nl");
		g_fail++;
	}
	free(got);
	assert_line("long_no_nl", 2, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 6: Newline falls exactly at end of buffer                   */
/*  BUFFER_SIZE=42 → 41 'b' + '\n' + 41 'c' + '\n'                  */
/*  First read consumes exactly one buffer: 41b + \n                  */
/*  Tests off-by-one in clear_storage / storage shifting              */
/* ================================================================== */
static void	attack_newline_at_buffer_end(void)
{
	char	content[87];
	char	exp1[43];
	char	exp2[43];
	int		fd;

	printf(CYN "--- ATTACK 6: Newline at buffer boundary ---\n" RST);
	memset(content, 'b', 41);
	content[41] = '\n';
	memset(content + 42, 'c', 41);
	content[83] = '\n';
	content[84] = '\0';
	memset(exp1, 'b', 41);
	exp1[41] = '\n';
	exp1[42] = '\0';
	memset(exp2, 'c', 41);
	exp2[41] = '\n';
	exp2[42] = '\0';
	fd = make_tmpfile("/tmp/gnl_t06.txt", content, 84);
	assert_line("nl_at_buf_end", 1, get_next_line(fd), exp1);
	assert_line("nl_at_buf_end", 2, get_next_line(fd), exp2);
	assert_line("nl_at_buf_end", 3, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 7: Multiple newlines within a single read                   */
/*  Content: "a\nb\nc\n" — fits entirely in one 42-byte read          */
/*  Expected: "a\n" "b\n" "c\n" NULL                                  */
/*  Tests correct storage shifting after each line extraction          */
/* ================================================================== */
static void	attack_multiple_nl_in_one_read(void)
{
	int	fd;

	printf(CYN "--- ATTACK 7: Multiple newlines in one read ---\n" RST);
	fd = make_tmpfile("/tmp/gnl_t07.txt", "a\nb\nc\n", -1);
	assert_line("multi_nl_one_read", 1, get_next_line(fd), "a\n");
	assert_line("multi_nl_one_read", 2, get_next_line(fd), "b\n");
	assert_line("multi_nl_one_read", 3, get_next_line(fd), "c\n");
	assert_line("multi_nl_one_read", 4, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 8: Invalid fd (-1)                                          */
/*  Expected: NULL                                                     */
/* ================================================================== */
static void	attack_invalid_fd(void)
{
	printf(CYN "--- ATTACK 8: Invalid fd (-1) ---\n" RST);
	assert_line("invalid_fd", 1, get_next_line(-1), NULL);
}

/* ================================================================== */
/*  ATTACK 9: Call on a closed fd                                      */
/*  Expected: NULL (read() returns -1 on closed fd)                   */
/*  Checks that stale static storage is not returned                  */
/* ================================================================== */
static void	attack_closed_fd(void)
{
	int		fd;
	char	*l;

	printf(CYN "--- ATTACK 9: Closed fd ---\n" RST);
	fd = make_tmpfile("/tmp/gnl_t09.txt", "line\n", -1);
	free(get_next_line(fd));
	close(fd);
	l = get_next_line(fd);
	if (l == NULL)
	{
		printf(GRN "[PASS]" RST " %-30s call #1 → NULL\n", "closed_fd");
		g_pass++;
	}
	else
	{
		printf(RED "[FAIL]" RST " %-30s call #1 → expected NULL, got [%s]\n",
			"closed_fd", l);
		free(l);
		g_fail++;
	}
}

/* ================================================================== */
/*  ATTACK 10: Two different fds (bonus behavior / pollution check)    */
/*  Standard GNL uses one static array → mixing fds causes UB         */
/*  This test verifies at minimum that no crash / segfault occurs     */
/*  and documents the non-deterministic output as expected             */
/* ================================================================== */
static void	attack_two_fds(void)
{
	int		fd1;
	int		fd2;
	char	*l1;
	char	*l2;

	printf(CYN "--- ATTACK 10: Two different fds (pollution check) ---\n" RST);
	printf(YEL "[INFO]" RST " Standard GNL assumes a single fd."
		" Interleaving is only correct in bonus.\n");
	fd1 = make_tmpfile("/tmp/gnl_t10a.txt", "file_one\n", -1);
	fd2 = make_tmpfile("/tmp/gnl_t10b.txt", "file_two\n", -1);
	l1 = get_next_line(fd1);
	l2 = get_next_line(fd2);
	if (l1)
	{
		printf("  fd1 → [%s]\n", l1);
		free(l1);
	}
	else
		printf("  fd1 → NULL\n");
	if (l2)
	{
		printf("  fd2 → [%s]\n", l2);
		free(l2);
	}
	else
		printf("  fd2 → NULL\n");
	printf(YEL "[INFO]" RST " Results above are implementation-defined"
		" for non-bonus GNL.\n");
	close(fd1);
	close(fd2);
}

/* ================================================================== */
/*  ATTACK 11: Null byte in the middle of content                      */
/*  ft_strlen stops at '\0' → content is truncated                    */
/*  Expected: implementation-defined, but must not crash               */
/* ================================================================== */
static void	attack_null_byte_in_content(void)
{
	int		fd;
	char	*l;

	printf(CYN "--- ATTACK 11: Null byte in content ---\n" RST);
	printf(YEL "[INFO]" RST " \"abc\\0def\\n\" — content after \\0 is invisible"
		" to str functions.\n");
	fd = make_tmpfile("/tmp/gnl_t11.txt", "abc\0def\n", 8);
	l = get_next_line(fd);
	if (l)
	{
		printf("  returned length: %d (full would be 8)\n", (int)strlen(l));
		if (strlen(l) < 8)
			printf(YEL "[WARN]" RST " ft_strlen truncated at null byte"
				" — expected behavior.\n");
		free(l);
	}
	else
		printf("  returned NULL\n");
	close(fd);
}

/* ================================================================== */
/*  ATTACK 12: 500 single-character lines                              */
/*  Content: "a\n" × 500 (1000 bytes total)                           */
/*  Expected: 500 calls each returning "a\n", then NULL               */
/*  Stress-tests clear_storage and cumulative memory correctness       */
/* ================================================================== */
static void	attack_500_single_char_lines(void)
{
	char	*content;
	int		fd;
	int		count;
	int		errors;
	char	*l;
	int		i;

	printf(CYN "--- ATTACK 12: 500 single-char lines ---\n" RST);
	content = malloc(1001);
	if (!content)
		return ;
	i = 0;
	while (i < 1000)
	{
		content[i] = 'a';
		content[i + 1] = '\n';
		i += 2;
	}
	content[1000] = '\0';
	fd = make_tmpfile("/tmp/gnl_t12.txt", content, 1000);
	free(content);
	count = 0;
	errors = 0;
	while ((l = get_next_line(fd)) != NULL)
	{
		count++;
		if (strcmp(l, "a\n") != 0)
			errors++;
		free(l);
	}
	if (count == 500 && errors == 0)
	{
		printf(GRN "[PASS]" RST " %-30s → 500 lines read correctly\n",
			"500_single_char");
		g_pass++;
	}
	else
	{
		printf(RED "[FAIL]" RST " %-30s → %d lines read, %d errors\n",
			"500_single_char", count, errors);
		g_fail++;
	}
	close(fd);
}

/* ================================================================== */
/*  ATTACK 13: Pipe (simulates small/unpredictable read sizes)         */
/*  Writes "hello\nworld" to a pipe, reads back via GNL               */
/*  Expected: "hello\n"  "world"  NULL                                 */
/* ================================================================== */
static void	attack_pipe_reads(void)
{
	int	pipefd[2];

	printf(CYN "--- ATTACK 13: Pipe reads ---\n" RST);
	if (pipe(pipefd) < 0)
	{
		perror("pipe");
		return ;
	}
	write(pipefd[1], "hello\nworld", 11);
	close(pipefd[1]);
	assert_line("pipe_reads", 1, get_next_line(pipefd[0]), "hello\n");
	assert_line("pipe_reads", 2, get_next_line(pipefd[0]), "world");
	assert_line("pipe_reads", 3, get_next_line(pipefd[0]), NULL);
	close(pipefd[0]);
}

/* ================================================================== */
/*  ATTACK 14: Single newline character only                           */
/*  Expected: "\n" then NULL                                           */
/* ================================================================== */
static void	attack_single_newline(void)
{
	int	fd;

	printf(CYN "--- ATTACK 14: Single newline only ---\n" RST);
	fd = make_tmpfile("/tmp/gnl_t14.txt", "\n", -1);
	assert_line("single_newline", 1, get_next_line(fd), "\n");
	assert_line("single_newline", 2, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 15: Reopen same file (static storage reset check)           */
/*  After reading a file to EOF and closing, re-open the same path    */
/*  and verify GNL reads from the beginning again without stale data  */
/* ================================================================== */
static void	attack_reopen_same_file(void)
{
	int	fd;

	printf(CYN "--- ATTACK 15: Reopen same file ---\n" RST);
	make_tmpfile("/tmp/gnl_t15.txt", "reopen\n", -1);
	fd = open("/tmp/gnl_t15.txt", O_RDONLY);
	assert_line("reopen_file", 1, get_next_line(fd), "reopen\n");
	assert_line("reopen_file", 2, get_next_line(fd), NULL);
	close(fd);
	fd = open("/tmp/gnl_t15.txt", O_RDONLY);
	assert_line("reopen_file", 3, get_next_line(fd), "reopen\n");
	assert_line("reopen_file", 4, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 16: stdin (fd = 0) — skip if not interactive               */
/*  Just verifies GNL accepts fd=0 without crashing in non-tty mode  */
/*  Actual reading from stdin is skipped to keep test non-blocking    */
/* ================================================================== */
static void	attack_fd_zero_info(void)
{
	printf(CYN "--- ATTACK 16: fd = 0 (STDIN) ---\n" RST);
	printf(YEL "[INFO]" RST " fd=0 (stdin) test skipped in automated mode"
		" — would block waiting for input.\n");
	printf(YEL "[INFO]" RST " To test manually: echo \"hello\" | ./gnl_test\n");
}

/* ================================================================== */
/*  ATTACK 17: Line of exactly BUFFER_SIZE characters (no newline)    */
/*  BUFFER_SIZE=42 → exactly 42 'z' chars, no newline                 */
/*  Expected: 42 'z' chars as one string, then NULL                   */
/* ================================================================== */
static void	attack_exact_buffer_size_no_nl(void)
{
	char	content[43];
	char	expected[43];
	int		fd;

	printf(CYN "--- ATTACK 17: Line = exactly BUFFER_SIZE, no newline ---\n" RST);
	memset(content, 'z', BUFFER_SIZE);
	content[BUFFER_SIZE] = '\0';
	memset(expected, 'z', BUFFER_SIZE);
	expected[BUFFER_SIZE] = '\0';
	fd = make_tmpfile("/tmp/gnl_t17.txt", content, BUFFER_SIZE);
	assert_line("exact_bufsz_no_nl", 1, get_next_line(fd), expected);
	assert_line("exact_bufsz_no_nl", 2, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 18: Line of exactly BUFFER_SIZE - 1 characters + newline   */
/*  BUFFER_SIZE=42 → 41 'q' + '\n' — newline is the last byte read   */
/* ================================================================== */
static void	attack_buffer_minus_one_then_nl(void)
{
	char	content[43];
	char	expected[43];
	int		fd;

	printf(CYN "--- ATTACK 18: BUFFER_SIZE - 1 chars + newline ---\n" RST);
	memset(content, 'q', BUFFER_SIZE - 1);
	content[BUFFER_SIZE - 1] = '\n';
	content[BUFFER_SIZE] = '\0';
	memset(expected, 'q', BUFFER_SIZE - 1);
	expected[BUFFER_SIZE - 1] = '\n';
	expected[BUFFER_SIZE] = '\0';
	fd = make_tmpfile("/tmp/gnl_t18.txt", content, BUFFER_SIZE);
	assert_line("bufsz_minus1_nl", 1, get_next_line(fd), expected);
	assert_line("bufsz_minus1_nl", 2, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 19: Alternating short and long lines                        */
/*  Tests that storage management handles variable-length lines well  */
/* ================================================================== */
static void	attack_alternating_line_lengths(void)
{
	int	fd;

	printf(CYN "--- ATTACK 19: Alternating short/long lines ---\n" RST);
	fd = make_tmpfile("/tmp/gnl_t19.txt",
		"x\n"
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n"
		"y\n"
		"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n",
		-1);
	assert_line("alt_lengths", 1, get_next_line(fd), "x\n");
	assert_line("alt_lengths", 2, get_next_line(fd),
		"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa\n");
	assert_line("alt_lengths", 3, get_next_line(fd), "y\n");
	assert_line("alt_lengths", 4, get_next_line(fd),
		"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb\n");
	assert_line("alt_lengths", 5, get_next_line(fd), NULL);
	close(fd);
}

/* ================================================================== */
/*  ATTACK 20: Consecutive calls after EOF                             */
/*  After NULL is returned, further calls must also return NULL        */
/*  (no crash, no garbage, no recycled data)                          */
/* ================================================================== */
static void	attack_call_after_eof(void)
{
	int	fd;

	printf(CYN "--- ATTACK 20: Multiple calls after EOF ---\n" RST);
	fd = make_tmpfile("/tmp/gnl_t20.txt", "only\n", -1);
	assert_line("call_after_eof", 1, get_next_line(fd), "only\n");
	assert_line("call_after_eof", 2, get_next_line(fd), NULL);
	assert_line("call_after_eof", 3, get_next_line(fd), NULL);
	assert_line("call_after_eof", 4, get_next_line(fd), NULL);
	close(fd);
}

/* ------------------------------------------------------------------ */
/*  MAIN                                                               */
/* ------------------------------------------------------------------ */
int	main(void)
{
	printf(CYN "\n╔══════════════════════════════════════════╗\n" RST);
	printf(CYN "║       GNL ATTACK TEST SUITE  v2.0        ║\n" RST);
	printf(CYN "║   get_next_line — 42 School stress test  ║\n" RST);
	printf(CYN "╚══════════════════════════════════════════╝\n\n" RST);
	printf("BUFFER_SIZE = %d\n\n", BUFFER_SIZE);

	attack_empty_file();             printf("\n");
	attack_only_newlines();          printf("\n");
	attack_no_trailing_newline();    printf("\n");
	attack_exactly_buffer_plus_one(); printf("\n");
	attack_long_line_no_newline();   printf("\n");
	attack_newline_at_buffer_end();  printf("\n");
	attack_multiple_nl_in_one_read(); printf("\n");
	attack_invalid_fd();             printf("\n");
	attack_closed_fd();              printf("\n");
	attack_two_fds();                printf("\n");
	attack_null_byte_in_content();   printf("\n");
	attack_500_single_char_lines();  printf("\n");
	attack_pipe_reads();             printf("\n");
	attack_single_newline();         printf("\n");
	attack_reopen_same_file();       printf("\n");
	attack_fd_zero_info();           printf("\n");
	attack_exact_buffer_size_no_nl(); printf("\n");
	attack_buffer_minus_one_then_nl(); printf("\n");
	attack_alternating_line_lengths(); printf("\n");
	attack_call_after_eof();         printf("\n");

	printf(CYN "══════════════════════════════════════════════\n" RST);
	printf("  Results: " GRN "%d passed" RST " / " RED "%d failed" RST
		" / %d total\n", g_pass, g_fail, g_pass + g_fail);
	printf(CYN "══════════════════════════════════════════════\n\n" RST);

	if (g_fail == 0)
		printf(GRN "All tests passed! Now run with valgrind:\n" RST);
	else
		printf(RED "Fix the failing tests, then run with valgrind:\n" RST);

	printf("  valgrind --leak-check=full --track-origins=yes"
		" --error-exitcode=1 ./gnl_test\n\n");

	printf(YEL "Notes:\n" RST);
	printf("  - ATTACK 10 (two fds): result is non-deterministic for standard GNL.\n");
	printf("    Correct interleaving is only required in the bonus part.\n");
	printf("  - ATTACK 11 (null byte): C string functions stop at '\\0'.\n");
	printf("    Truncated output is expected behavior, not a bug.\n");
	printf("  - ATTACK 16 (stdin): skipped in automated mode to avoid blocking.\n\n");

	return (g_fail > 0 ? 1 : 0);
}